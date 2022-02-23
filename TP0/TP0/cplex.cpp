#include "cplex.hpp"
#include "network.hpp"

double rjesenje_cplex(Network* graf, int flow_value, Flow* flow) {
    IloEnv CPLEX_okruzje;
    try {
        
        // definicija okruzja za rjesenje
        IloModel CPLEX_model(CPLEX_okruzje);

        int br_bridova = 0;
        for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
            it != graf->costs.end(); ++it)
            ++br_bridova;

        IloNumVarArray varijabla_x(CPLEX_okruzje, br_bridova, 0, IloInfinity,
            ILOINT);  // funkcija koja bira tok
        IloNumVarArray varijabla_y(
            CPLEX_okruzje, br_bridova, 0, 1,
            ILOINT);  // funkcija koja odreduje je li minimum varijabilan ili fiksan

        // funkcija cilja

        IloExpr CPLEX_fja_cilja(CPLEX_okruzje);

        int i = 0;
        for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
            it != graf->costs.end(); ++it) {
 
            CPLEX_fja_cilja += it->second * varijabla_x[i];
            ++i;
        }

        CPLEX_model.add(IloMinimize(CPLEX_okruzje, CPLEX_fja_cilja));

        // ogranicenja

        // 2

        IloExpr ogr2(CPLEX_okruzje);
        i = 0;
        std::pair<vertex_key, vertex_key> par_vrhova;

        for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
            it != graf->costs.end(); ++it) {

            par_vrhova = get_vertex_keys(it->first);
            //if (par_vrhova.first == 0) CPLEX_ogranicenje += varijabla_x[i];
            if(par_vrhova.first == 0) ogr2 += varijabla_x[i];
            ++i;
        }

        CPLEX_model.add(ogr2 <= (int)flow_value);
        CPLEX_model.add(ogr2 >= (int)flow_value);
        ogr2.end();

        // 3

        IloExprArray ogr3(CPLEX_okruzje, br_bridova);
        for (int z = 0; z < br_bridova; ++z) {
            ogr3[z] = IloExpr(CPLEX_okruzje);
        }
        int current = 0;

        for (vertex_key pivotni_vrh = 1; pivotni_vrh < graf->sink; ++pivotni_vrh) {

            i = 0;

            for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
                it != graf->costs.end(); ++it) {

                par_vrhova = get_vertex_keys(it->first);

                if (par_vrhova.first == pivotni_vrh)
                    ogr3[current] += varijabla_x[i];

                else if (par_vrhova.second == pivotni_vrh)
                    ogr3[current] -= varijabla_x[i];
                ++i;
            }

            CPLEX_model.add(ogr3[current] <= 0);
            CPLEX_model.add(ogr3[current] >= 0);
            ++current;
        }

        ogr3.end();

        // 4

        IloExpr ogr4(CPLEX_okruzje);

        i = 0;

        for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
            it != graf->costs.end(); ++it) {

            par_vrhova = get_vertex_keys(it->first);

            if (par_vrhova.second == graf->sink) ogr4 += varijabla_x[i];
            ++i;
        }

        CPLEX_model.add(ogr4 <= (int)flow_value);
        CPLEX_model.add(ogr4 >= (int)flow_value);

        ogr4.end();
        // 5

        IloExprArray ogr5(CPLEX_okruzje, br_bridova);
        for (int z = 0; z < br_bridova; ++z) {
            ogr5[z] = IloExpr(CPLEX_okruzje);
        }
        current = 0;

        i = 0;
        for (std::unordered_map<edge_key, int>::iterator it = graf->minimum_quantities.begin();
            it != graf->minimum_quantities.end(); ++it) {

            if (graf->vlbs.find(it->first) != graf->vlbs.end()) ogr5[current] += varijabla_x[i] - it->second * varijabla_y[i];
            else ogr5[current] += varijabla_x[i] - it->second;

            CPLEX_model.add(ogr5[current] >= 0);
            ++current;
            ++i;
        }

        ogr5.end();

        // 6

        IloExprArray ogr6(CPLEX_okruzje, br_bridova);
        for (int z = 0; z < br_bridova; ++z) {
            ogr6[z] = IloExpr(CPLEX_okruzje);
        }
        current = 0;

        i = 0;

        for (std::unordered_map<edge_key, int>::iterator it = graf->capacities.begin();
            it != graf->capacities.end(); ++it) {

            if (graf->vlbs.find(it->first) != graf->vlbs.end()) ogr6[current] += varijabla_x[i] - it->second * varijabla_y[i];
            else ogr6[current] += varijabla_x[i] - it->second;

            CPLEX_model.add(ogr6[current] <= 0);
            ++current;

            ++i;
        }

        ogr6.end();

        // 7 je zadovoljeno modelom

        IloCplex CPLEX_rjesenje(CPLEX_model);
        CPLEX_rjesenje.setOut(CPLEX_okruzje.getNullStream());
        if (!CPLEX_rjesenje.solve()) {
            CPLEX_okruzje.error() << "Rjesenje nije pronadeno" << std::endl;
            return -1;
        }
        std::cout << "CPLEX zapisao ogranicenja, krece racun" << std::endl;
        double CPLEX_vrijednost = CPLEX_rjesenje.getObjValue();

        int l = 0;

        for (auto it = graf->costs.begin(); it != graf->costs.end(); ++it) {
            vertex_key prvi = get_vertex_keys(it->first).first, drugi = get_vertex_keys(it->first).second;
            flow->add_edge(prvi, drugi, CPLEX_rjesenje.getValue(varijabla_x[l]));
            ++l;
        }


        varijabla_x.end();
        varijabla_y.end();
        CPLEX_fja_cilja.end();
        CPLEX_okruzje.end();

        return CPLEX_vrijednost;
    }
    catch (IloException& e) {
        std::cout << "Concert Exception: " << e << std::endl;
    }
    CPLEX_okruzje.end();
    return -1;
}