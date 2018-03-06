#include <iostream>
#include "memory.h"
#include <unistd.h>

using namespace std;

const int N = 6, A = 0, B = A+N*N, C = B+N*N;

int main ()
{
    Memory m;
    int ii=0;

    cout << "Naive matrix multiplication (N = " << N << ")\n";


    for (ii = 0; ii <= 1; ii++)
    {
        printCacheOrg(ii);
        m.resetClock();

        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                for (int k = 0; k < N; k++) {
                    int c = m.getData(C + i * N + j);
                    int a = m.getData(A + i * N + k);
                    int b = m.getData(B + k * N + j);
                    c += a * b;
                    m.putData(C + i * N + j, c);
                }

        float missrate = (float) m.getNumCacheMisses() / (float) (N * N * N * 4) * 100.0;
        cout << "\nClock : " << m.getClock()
             << " Cache misses: " << m.getNumCacheMisses();
        cout.setf(ios::fixed);
        cout.precision(2);
        cout << " Cache miss rate(%): " << missrate << endl;

        // print cache contents
        m.showCacheAddress(ii);
        cout<<"=====================================\n ";
    }

    return 0;
}

