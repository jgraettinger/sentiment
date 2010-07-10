
#include <armadillo>
#include <iostream>

using namespace arma;
using namespace std;

int main(int argc, char ** argv)
{

    mat X(
        "1 0 0 0 2;"
        "0 0 3 0 0;"
        "0 0 0 1 0;"
        "0 4 0 0 0;"
        "3 2 0 0 1;");

    mat U;
    vec s;
    mat V;
    bool status = svd(U,s,V,X);

    cout << "input: " << endl;
    X.print();
    cout << endl;

    cout << "iteration: " << endl;
    for(mat::iterator it = X.begin(); it != X.end(); ++it)
        cout << *it << endl;
    cout << endl;

    mat S = zeros(U.n_rows, V.n_cols);
    S.diag(0) = s;


    cout << "eigenvalues:" << endl;
    s.print();
    cout << endl;

    cout << "determinant: " << prod(s) << endl;

    mat tmp = (U * S * trans(V));
    
    cout << "rebuilt input: " << endl;
    tmp.print();
    cout << endl;

    // Computing the inverse given SVD decomp

    vec inv_s = 1 / s;

    mat inv_S = zeros(U.n_rows, V.n_cols);
    inv_S.diag(0) = inv_s;

    mat covar_inv = (V * inv_S * trans(U));

    cout << "ident: " << endl;
    tmp = (V * inv_S * trans(U)) * X;
    tmp.print();
    cout << endl;

    return 0;
}
