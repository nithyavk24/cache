#include <iostream>
#include "memory.h"

using namespace std;

const int N = 16, A = 0, B = A+N*N, C = B+N*N;

int main ()
{
    int ii;
    Memory m;

    cout << "Naive matrix multiplication (N = " << N << ")\n";

    // iterate over cache organization: direct, twoway, fully
    for (ii =0; ii <=1;ii++) {

        printCacheOrg (ii);
        m.resetClock(); // reset clock and numMisses

        for ( int i = 0; i < N; i++ )
            for ( int j = 0; j < N; j++ )
                for ( int k = 0; k < N; k++ ) {
                    int c = m.getData ( C + i*N + j );
                    int a = m.getData ( A + i*N + k );
                    int b = m.getData ( B + k*N + j );
                    c += a * b;
                    m.putData ( C + i*N + j, c );
                }

        float missrate = (float)numMisses/(float)(N*N*N*4)*100.0;
        cout << "\nClock : " << clockX
             << " Cache misses: " << numMisses;
        cout.setf(ios::fixed);
        cout.precision(2);
        cout << " Cache miss rate(%): " << missrate << endl;

        // print cache contents
        m.showCacheAddress(ii);
        cout<<"======================================\n";
    }

    return 0;
}


