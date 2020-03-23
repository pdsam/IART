#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

enum orientation
{
    H,
    V
};

struct Image
{

    orientation ori;
    int numberTags;
    vector<string> tags;
};



vector<Image *> loadInput(int input)
{
    ifstream file;
    string line;
    vector<Image *> photos;
    string files[] = {
        "../input/a_example.txt",
        "../input/b_lovely_landscapes.txt",
        "../input/c_memorable_moments.txt",
        "../input/d_pet_pictures.txt",
        "../input/e_shiny_selfies.txt"};

    file.open(files[input]);

    getline(file, line);
    while (getline(file, line))
    {
        string orientation, numberTags;
        Image *image = new Image();
        istringstream iss(line);
        iss >> orientation >> numberTags;

        (orientation == "H") ? image->ori = H : image->ori = V;
        image->numberTags = stoi(numberTags);

        while (iss)
        {
            string tag;
            iss >> tag;
            image->tags.push_back(tag);

        }
            sort(image->tags.begin(),image->tags.end());
        photos.push_back(image);

    }
        file.close();
        return photos;
}

int main()
{
    loadInput(0);
    return 0;
}
