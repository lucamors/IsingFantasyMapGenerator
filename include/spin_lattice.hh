#ifndef SPIN_LATTICE_H
#define SPIN_LATTICE_H

class SpinLattice{

    public:

        SpinLattice(int N);
        ~SpinLattice();

        int& operator()(int i, int j);
        
        int get_size();
        

        void anneal(int);
        void freeze();

    private:

        void init_lattice();

        // Energy related functions
        float evaluate_energy();
        float compute_cell_energy_contribution(int, int);
        float evaluate_energy_from_previous(int, int, float);

        int** lattice;

        // Lattice Properties
        int lattice_size;
        int boarder_size;

        // Hamiltonian parameters
        float J = 1; // spin-spin coupling constant
        float temperature = 3; // system temperature 
        
        float energy;

};

#endif