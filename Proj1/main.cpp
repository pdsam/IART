#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
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

unsigned int evaluation(const vector<Image*> &images){

	if(images.size() < 2)
		return 0;

	auto left = images.begin();
	auto right = left + ((*left)->ori == V ? 2 : 1);

	unsigned points = 0;
	while(right != images.end()){

		int distance = (*left)->ori == V? 2 : 1;
		assert( (left+distance)==right );

		#define VERTICAL_FOLLOWS_VERTICAL(x) if((*x)->ori == V) assert((*(x+1))->ori == V);
		VERTICAL_FOLLOWS_VERTICAL(left)
		VERTICAL_FOLLOWS_VERTICAL(right)

		auto left_tags = new vector<string>((*left)->tags);
		auto right_tags = new vector<string>((*right)->tags);

		#define GET_TAGS(x, y) \
			if((*x)->ori == V) {\
				y->reserve(y->size() + (*(x+1))->tags.size());\
				y->insert(y->end(), (*(x+1))->tags.begin(), (*(x+1))->tags.end());\
			}

		GET_TAGS(left, left_tags);
		GET_TAGS(right, right_tags);
			
		vector<string> unique_tags;
		set_intersection(left_tags->begin(),
				left_tags->end(),
				right_tags->begin(),
				right_tags->end(),
				unique_tags.begin());


		int unique_tags_count = unique_tags.size();
		points += min( static_cast<int>(right_tags->size())-unique_tags_count ,min(unique_tags_count, static_cast<int>(left_tags->size())-unique_tags_count));
		left += ((*left)->ori == V ? 2 : 1);
		right += ((*right)->ori == V ? 2 : 1);
	}	

	return points;
}

int main(){

	loadInput(0);
	return 0;
}
