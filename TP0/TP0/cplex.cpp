#include "cplex.hpp"

double rjesenje_cplex(FlowNetwork* graf) {


    // definicija okruzja za rjesenje
    IloEnv CPLEX_okruzje;
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

    IloExpr CPLEX_ogranicenje(CPLEX_okruzje);

    // 2

    i = 0;
    std::pair<vertex_key, vertex_key> par_vrhova;

    for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
        it != graf->costs.end(); ++it) {

        par_vrhova = get_vertex_keys(it->first);
        if (par_vrhova.first == 0) CPLEX_ogranicenje += varijabla_x[i];
        ++i;
    }

    CPLEX_model.add(CPLEX_ogranicenje <= (int)graf->flow_value);
    CPLEX_model.add(CPLEX_ogranicenje >= (int)graf->flow_value);

    // 3

    for (vertex_key pivotni_vrh = 2; pivotni_vrh < graf->n_nodes; ++pivotni_vrh) {
        CPLEX_ogranicenje -= CPLEX_ogranicenje;

        i = 0;

        for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
            it != graf->costs.end(); ++it) {

            par_vrhova = get_vertex_keys(it->first);

            if (par_vrhova.first == pivotni_vrh)
                CPLEX_ogranicenje += varijabla_x[i];

            else if (par_vrhova.second == pivotni_vrh)
                CPLEX_ogranicenje -= varijabla_x[i];

            ++i;
        }

        CPLEX_model.add(CPLEX_ogranicenje <= 0);
        CPLEX_model.add(CPLEX_ogranicenje >= 0);
    }

    // 4

    i = 0;
    CPLEX_ogranicenje -= CPLEX_ogranicenje;

    for (std::unordered_map<edge_key, int>::iterator it = graf->costs.begin();
        it != graf->costs.end(); ++it) {

        par_vrhova = get_vertex_keys(it->first);
        if (par_vrhova.second == 1) CPLEX_ogranicenje += varijabla_x[i];
        ++i;
    }

    CPLEX_model.add(CPLEX_ogranicenje <= (int)graf->flow_value);
    CPLEX_model.add(CPLEX_ogranicenje >= (int)graf->flow_value);

    // 5

    i = 0;
    std::unordered_map<edge_key, bool>::iterator varijabilnost = graf->vlb.begin();
    for (std::unordered_map<edge_key, int>::iterator it = graf->minimum_quantities.begin();
        it != graf->minimum_quantities.end(); ++it) {

        CPLEX_ogranicenje -= CPLEX_ogranicenje;

        if (varijabilnost->second) CPLEX_ogranicenje += varijabla_x[i] - it->second * varijabla_y[i];
        else CPLEX_ogranicenje += varijabla_x[i] - it->second;

        CPLEX_model.add(CPLEX_ogranicenje >= 0);

        ++i;
        ++varijabilnost;
    }

    // 6

    i = 0;
    varijabilnost = graf->vlb.begin();
    for (std::unordered_map<edge_key, int>::iterator it = graf->capacities.begin();
        it != graf->capacities.end(); ++it) {

        CPLEX_ogranicenje -= CPLEX_ogranicenje;

        if (varijabilnost->second) CPLEX_ogranicenje += varijabla_x[i] - it->second * varijabla_y[i];
        else CPLEX_ogranicenje += varijabla_x[i] - it->second;

        CPLEX_model.add(CPLEX_ogranicenje <= 0);

        ++i;
        ++varijabilnost;
    }

    // 7 je zadovoljeno modelom

    IloCplex CPLEX_rjesenje(CPLEX_model);
    CPLEX_rjesenje.setOut(CPLEX_okruzje.getNullStream());
    if (!CPLEX_rjesenje.solve()) {
        CPLEX_okruzje.error() << "Rjesenje nije pronadeno" << std::endl;
        throw(-1);
    }

    double CPLEX_vrijednost = CPLEX_rjesenje.getObjValue();

    return CPLEX_vrijednost;
}