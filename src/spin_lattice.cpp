#include "spin_lattice.hh"
#include "utils.hh"
#include <random>
#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SpinLattice::SpinLattice(int N)
{
    lattice_size = N;
    boarder_size = ((int)(0.05*N));

    // Create the lattice grid
    init_lattice();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

SpinLattice::~SpinLattice(){

    for (size_t i = 0; i < lattice_size; i++)
    {
        delete[] lattice[i];
    }

    delete[] lattice;
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


int& SpinLattice::operator()(int i, int j) 
{
        return lattice[i][j];
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

int SpinLattice::get_size()
{
    return lattice_size;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SpinLattice::init_lattice()
{

    // Create an NxN lattice    
    lattice = new int*[lattice_size];
    for (auto i = 0; i < lattice_size; i++)
    {
        lattice[i] = new int[lattice_size];
    }

    // Init PRNG engine
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 1);

    // Initialize values at random
    for (int i = 0; i < lattice_size; i++) {
        for (int j = 0; j < lattice_size; j++) {
 
            // Assign values to the
            // memory blocks created
            int spin = (distribution(gen) == 0) ? -1 : 1;

            if(i < boarder_size or j < boarder_size or i > lattice_size-boarder_size or j > lattice_size-boarder_size) 
            {
                spin = -1;
            };

            lattice[i][j] = spin;

        }
    }

    // Compute the energy for the initial lattice configuration
    energy = evaluate_energy();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SpinLattice::anneal(int iter_number){

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(boarder_size, lattice_size-boarder_size);
    std::uniform_real_distribution<float> random_uniform(0, 1);

    for(int k = 0; k < iter_number; k++)
    {
        if(k%1000==0) loading_bar(k+1,iter_number);
        if(k == iter_number-1) loading_bar(k+1,iter_number);
        
        // Choose a cell at random
        int x = distribution(gen);
        int y = distribution(gen);

        // take a spin flip
        lattice[x][y] *= -1;

        float new_energy = evaluate_energy_from_previous(x,y,energy);        

        // Metropolis-Hasting procedure
        if(new_energy < energy){
            energy = new_energy;
        }
        else{

            // random number between 0 and 1
            float u = random_uniform(gen);

            float p = exp(-(new_energy-energy)/temperature);

            if(p > u){
                energy = new_energy;
            }
            else{
                // restore the spin back to its origin position
                lattice[x][y] *= -1;
            }
        }

        // Update temperature 
        // std::cout << "iter :"  << k << ", temp : " << temperature << std::endl;
        if(k % 100000 == 0)
        {
            temperature *= 0.99;
        }

    }

    return ;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void SpinLattice::freeze()
{
    // Setting temperature to 0 and run again the monte carlo procedure
    temperature = 1e-10;
    anneal(100000);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

float SpinLattice::evaluate_energy_from_previous(int x, int y, float previous_energy)
{
    lattice[x][y] *= -1;
    float new_energy = previous_energy;

    float center_energy_contribution = compute_cell_energy_contribution(x,y);
    float top_energy_contribution    = compute_cell_energy_contribution(x,y+1);
    float bottom_energy_contribution = compute_cell_energy_contribution(x,y-1);
    float right_energy_contribution  = compute_cell_energy_contribution(x+1,y);
    float left_energy_contribution   = compute_cell_energy_contribution(x-1,y);
    
    lattice[x][y] *= -1;

    float new_center_energy_contribution = compute_cell_energy_contribution(x,y);
    float new_top_energy_contribution    = compute_cell_energy_contribution(x,y+1);
    float new_bottom_energy_contribution = compute_cell_energy_contribution(x,y-1);
    float new_right_energy_contribution  = compute_cell_energy_contribution(x+1,y);
    float new_left_energy_contribution   = compute_cell_energy_contribution(x-1,y);
    
    new_energy += (new_top_energy_contribution-top_energy_contribution);
    new_energy += (new_bottom_energy_contribution-bottom_energy_contribution);
    new_energy += (new_right_energy_contribution-right_energy_contribution);
    new_energy += (new_left_energy_contribution-left_energy_contribution); 

    return new_energy;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

float SpinLattice::compute_cell_energy_contribution(int i, int j)
{
    // 1-cell contribution to the Ising Hamiltonian with periodic-boundary condition
    return -J*0.5*lattice[(i+lattice_size)%lattice_size][(j+lattice_size)%lattice_size] * ( lattice[(i+lattice_size)%lattice_size][(j+lattice_size)%lattice_size] + lattice[(i-1 + lattice_size) % lattice_size][(j+lattice_size)%lattice_size] + lattice[(i+1+lattice_size)%lattice_size][(j+lattice_size)%lattice_size] + lattice[(i+lattice_size)%lattice_size][(j-1+lattice_size)%lattice_size] + lattice[(i+lattice_size)%lattice_size][(j+1+lattice_size)%lattice_size] );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

float SpinLattice::evaluate_energy()
{
    float current_energy = 0;

    for(auto i = 0; i < lattice_size; i++)
    {
        for(auto j = 0; j < lattice_size; j++)
        {
            current_energy += -J*0.5*lattice[i][j] * ( lattice[i][j] + lattice[(i-1 + lattice_size) % lattice_size][j] + lattice[(i+1+lattice_size)%lattice_size][j] + lattice[i][(j-1+lattice_size)%lattice_size] + lattice[i][(j+1+lattice_size)%lattice_size] );
        }
    }
    

    return current_energy;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
