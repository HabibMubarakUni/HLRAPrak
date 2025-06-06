/*
user input n Typ: integer
wenn 0 <= n <= 9 -> print Hello world! n-mal
*/ 

# include <iostream>

int main(){
    int n;
    std::cout << "Geben Sie eine Zahl n zwischen 0 und 9: ";
    std::cin >> n;

    if (std::cin.fail()){
        std::cout << "Eingabe war keine Zahl" << std::endl;
        return 1;
    }

    if(0<=n && n <=9){
        for(int i=0; i<n; i++){
            std::cout << "Hello world!" << std::endl;
        }
    }
    else{
        std::cout << "n liegt nicht zwischen 0 und 9." << std::endl;
    }
    return 0;
}