/* To use the VCL C++ Vector Class Library:
   - Download the source code (and manual) of the latest release from GitHub
        https://www.agner.org/optimize/#vectorclass
   - Unpack the source code (zip or tar.gz) into a directory named version2-2.01.03
        (or whatever is the latest version)
   - Compile your VCL vectorized program with 
        c++ -std=c++17 Nbody.cpp -I./version2-2.01.03 -o Nbody
   - Set the path in the -I compiler flag to point to the directory where VCL is
     (in this example it is in the current directory ./ )
*/

// include VCL library
#include "vectorclass.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

const double G  = 6.67259e-7;  /* Gravitational constant (should be e-10 but modified to get more action */
const double dt = 1.0;         /* Length of timestep */


// Distance between points with coordinates (px,py) and (qx,qy)
double dist(double px, double py, double qx, double qy)
{
    return sqrt (pow(px-qx, 2) + pow(py-qy, 2));
    // Could also be written as sqrt( (px-qx)*(px-qx) + (py-qy)*(py-qy) )
}


/* Computes forces between bodies */
void ComputeForce(int N, double *X, double *Y, double *mass, double *Fx, double *Fy)
{
    const double mindist  = 0.0001;     /* Minimal distance of two bodies of being in interaction*/

    for (int i = 0; i < N; i++)         // Compute the force for all bodies
    {
        Fx[i] = Fy[i] = 0.0;            // Initialize force vector to zero
        for (int j = 0; j < N; j++)     // The force on a body i is the sum of forces from all other bodies j
        {
            if (i != j)                 // but not from it self
            {
               // Distance between points i and j
               double r = dist(X[i], Y[i], X[j], Y[j]);

               if (r > mindist)         // Very near-distance forces are ignored
               {
                   double r3 = pow(r, 3);     // Could also be written as r3 = r*r*r;
                   Fx[i] += G * mass[i] * mass[j] * (X[j]-X[i]) / r3;
                   Fy[i] += G * mass[i] * mass[j] * (Y[j]-Y[i]) / r3;
               }
            }
        }
    }
}

/* Computes forces between bodies using VCL library*/
void ComputeForceVCL(int N, double *X, double *Y, double *mass, double *Fx, double *Fy)
{
    // Using double precision type: let's use 256-bit vector with 4 doubles (AVX minimum instruction set)
    const int vecLen = 4; // 256-bit AVX, double data type

    // Declare vectors
    Vec4d Xvec, Yvec, Mvec, Fxvec, Fyvec;

    // Minimal distance of two bodies of being in interaction
    const double mindist  = 0.0001;
    // Arrays of force values to store result of the computation
    double *fx[N] = (double *) calloc(N, sizeof(double));
    double *fy[Y] = (double *) calloc(N, sizeof(double));

    // For every bodies
    for (int i = 0; i < N; i++)
    {
        // Take the x, y, and mass values from one particle Pi
        Vec4d XvecI = X[i];      // Duplicate Xi to vector
        Vec4d YvecI = Y[i];      // Duplicate Yi to vector
        Vec4d MvecI = mass[i];   // Duplicate MASSi to vector

        // For every possible vectors (nbbodies / veclen)
        for (int j = 0; j < N/vecLen; j++)
        {
            if (i != j)
            {
                // Distance between points i and j
                double r = dist(X[i], Y[i], X[j], Y[j]);
                if (r > mindist)
                {
                    double r3 = pow(r, 3);

                    // Load vectors
                    Xvec.load  (&X[i]);             // Load X vector
                    Yvec.load  (&Y[i]);             // Load Y vector
                    Mvec.load  (&mass[i]);          // Load mass vector

                    // Compute the `veclen` force values for bodie i
                    Fxvec = G * MvecI * Mvec * (Xvec - XvecI) / r3;
                    Fyvec = G * MvecI * Mvec * (Yvec - YvecI) / r3;

                    // Store these values
                    Fxvec.store(&fx[i*vecLen]);
                    Fyvec.store(&fy[i*vecLen]);
                }
            }
        }

        // Sum all the forces that the particle pi experiences from all other particles
        double fxSum = 0.0; double fySum = 0.0;
        for (int j = 0; j < N; j++)
        {
            fxSum += Fx[i];
            fySum += Fy[i];
        }
        // Store the global force in Fx and Fy
        Fx[i] = fxSum;
        Fy[i] = fySum;
    }

    // Free...
    free(fx); free(fy);
}

int main(int argc, char **argv)
{

    const int N = 1000;                // Number of bodies
    const int timesteps = 50;        // Number of timesteps
    const double size = 100.0;         // Initial positions are in the range [0,100]

    double *mass;          /* mass of bodies */
    double *X;             /* x-positions of bodies */
    double *Y;             /* y-positions of bodies */
    double *X_old;         /* old x-positions of bodies */
    double *Y_old;         /* old y-positions of bodies */
    double *Vx;            /* velocities on x-axis of bodies */
    double *Vy;            /* velocities on y-axis of bodies */
    double *Fx;            /* forces on x-axis of bodies */
    double *Fy;            /* forces on y-axis of bodies */

    printf ("N-body simulation, number of bodies = %d \n", N);

    /* Allocate space for variables  */
    mass     = (double *) calloc(N, sizeof(double));  // Mass
    X        = (double *) calloc(N, sizeof(double));  // Position (x,y) at current time step
    Y        = (double *) calloc(N, sizeof(double));
    X_old    = (double *) calloc(N, sizeof(double));  // Position (x,y) at previous time step
    Y_old    = (double *) calloc(N, sizeof(double));
    Vx       = (double *) calloc(N, sizeof(double));  // Velocities
    Vy       = (double *) calloc(N, sizeof(double));
    Fx       = (double *) calloc(N, sizeof(double));  // Forces
    Fy       = (double *) calloc(N, sizeof(double));


    // Seed the random number generator to make reproducable results
    //short unsigned int seedval[3] = {7, 7, 7};
    //seed48(seedval);

    /* Initialize mass and position of bodies */
    for (int i = 0; i<N; i++)
    {
        mass[i]  = 10000.0*rand(); //drand48();
        X_old[i] = size*rand(); //drand48();
        Y_old[i] = size*rand(); //drand48();
    }

    // Save position of one body so we can see where it has moved
    double pos0x = X_old[0];
    double pos0y = Y_old[0];

    // Compute the initial forces that we get
    ComputeForceVCL(N, X_old, Y_old, mass, Fx, Fy);

    // Set up the velocity vectors caused by initial forces
    for(int i = 0; i < N; i++)
    {
        Vx[i] = -0.5 * dt * Fx[i] / mass[i];
        Vy[i] = -0.5 * dt * Fy[i] / mass[i];
    }


    /* Main loop:
       - Move the bodies
       - Calculate forces of the bodies with their new position
       - Calculate velocities of the bodies with the new forces
       - Copy the updated positions to the old positions (for use in next timestep)
     */
    int t = 0;
    while (t < timesteps)                   // Loop for this many timesteps
    {
        t++;
        printf("%d ", t); fflush(stdout);   // Print out the timestep so there is something to look at

        // Calculate new positions
        for (int i = 0; i < N; i++)
        {
            X[i] = X_old[i] + Vx[i]*dt;     // Old position + the distance it has moved in this timestep
            Y[i] = Y_old[i] + Vy[i]*dt;
        }

        /* Calculate forces for the new positions */
        ComputeForceVCL(N, X_old, Y_old, mass, Fx, Fy);

        /* Update velocities of bodies */
        for (int i = 0; i < N; i++)
        {
            Vx[i] = Vx[i] + Fx[i] * dt / mass[i];
            Vy[i] = Vy[i] + Fy[i] * dt / mass[i];
        }

        /* Copy updated positions to (X_old, Y_old) before next time step begins */
        for (int i = 0; i < N; i++)
        {
            X_old[i] = X[i];
            Y_old[i] = Y[i];
        }
    }  /* end of while-loop */
    printf("\n");


    //DEBUG
    double pos0_finalx = X_old[0];
    double pos0_finaly = Y_old[0];
    printf("Original pos %3.8f %3.8f\n", pos0x, pos0y);
    printf("Final pos    %3.8f %3.8f\n", pos0_finalx, pos0_finaly);

    // Free ...
    free(mass); free(X); free(Y); free(X_old); free(Y_old);
    free(Vx); free(Vy); free(Fx); free(Fy);

    exit(0);
}
