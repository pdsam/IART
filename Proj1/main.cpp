#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Image{
    int orientation; // H = 0 ; V = 1
    int id;
    int numberTags;
    vector<string> tags;

};


vector<Image> loadInput(int input){
    ifstream file;
    string line;
    vector<Image> photos;

    switch(input){
        case 0:
        file.open("input/a_example.txt");
            break;
        case 1:
            file.open("input/b_lovely_landscapes.txt");
            break;
        case 2:
            file.open("input/c_memorable_moments.txt");
            break;
        case 3:
            file.open("input/d_pet_pictures.txt");
            break;
        case 4:
            file.open("input/e_shiny_selfies.txt");
            break;

    }


    file.close();
    return photos;
}


int main(){





}