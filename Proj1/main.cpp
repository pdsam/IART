#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <map>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;


enum Orientation
{
    H,
    V
};
struct Image
{
    Orientation orientation;
    set<int> tags;
};


struct Slide{

        union{
                Image* image;
                Image* vert_images[2];
        };

		unordered_set<int> all_tags;

        bool is_vertical() {
                return image->orientation == V;
        }

		void do_tags(){
			all_tags.clear();
			all_tags.insert(vert_images[0]->tags.begin(), vert_images[0]->tags.end());
			if(!is_vertical()){
				return;
			}
			all_tags.insert(vert_images[1]->tags.begin(), vert_images[1]->tags.end());
		}

        auto get_tags(){
                return all_tags;
        }

        int number_tags(){
                return get_tags().size();
        }
};

typedef vector<Slide*> SlideShow;


pair<SlideShow, SlideShow> loadInput(int input)
{
    SlideShow slideshow;
        vector<Image*> vertical_photos;

    ifstream file;
    string line;
    static const string files[] = {
        "input/a_example.txt",
        "input/b_lovely_landscapes.txt",
        "input/c_memorable_moments.txt",
        "input/d_pet_pictures.txt",
        "input/e_shiny_selfies.txt"};

    file.open(files[input]);
    getline(file, line);
	map<string,int> simplifier;
	auto counter = 0;
	while (getline(file, line))
    {
        string orientation, _;

        Image *image = new Image();
        istringstream iss(line);
        iss >> orientation >> _;

        image->orientation = (orientation == "H" ? H :  V);

        while (iss)
        {
            string tag;
            iss >> tag;

			auto cur = simplifier.find(tag);
			if(cur == simplifier.end()){
				auto res = simplifier.insert(make_pair(tag, counter++));
				cur = get<0>(res);
			}
            image->tags.insert(cur->second);

        }

		if(image->orientation == H){
				auto slide = new Slide();
				slide->image = image;
				slideshow.push_back(slide);
		}
		else{
				vertical_photos.push_back(image);
		}
    }
    file.close();

    vector<Slide*> vertical_slides;
    assert(vertical_photos.size()%2==0);
    while(vertical_photos.size()){
            auto slide = new Slide();
            slide->vert_images[0] = vertical_photos.back();
            vertical_photos.pop_back();
            slide->vert_images[1] = vertical_photos.back();
            vertical_photos.pop_back();

            slideshow.push_back(slide);
            vertical_slides.push_back(slide);
    }

    for(auto &element : slideshow)
        element->do_tags();

    return make_pair(slideshow, vertical_slides);
}

unsigned int evaluation(const SlideShow &slideshow){

        if(slideshow.size() < 2)
                return 0;

        auto left = slideshow.begin();
        auto right = left + 1;

        unsigned points = 0;
        while(right != slideshow.end()){

                auto left_tags = (*left)->get_tags();
                auto right_tags = (*right)->get_tags();

                const auto small_to_big = (left_tags.size() > right_tags.size() ? make_pair(right_tags,left_tags) : make_pair(left_tags, right_tags));
                int common_tags = 0;

                for(const auto &element : get<0>(small_to_big)){
                        if(get<1>(small_to_big).find(element) != get<1>(small_to_big).end())
                                common_tags +=1;
                }

                points += min((*left)->number_tags()-common_tags, min(common_tags, (*right)->number_tags()-common_tags));
                left++;
                right++;
        }

        return points;
}


namespace Operator{

        SlideShow swap_slides(SlideShow &slideshow, int i, int j){
                if(i == j)
                        return slideshow;
                swap(slideshow[i], slideshow[j]);
                return slideshow;
        }

        SlideShow swap_verticals(SlideShow &slideshow, int i, int j) {
            if (!slideshow[i]->is_vertical() || !slideshow[j]->is_vertical()) {
                return slideshow;
            }

            swap(slideshow[i]->vert_images[1], slideshow[j]->vert_images[1]);
            slideshow[i]->do_tags();
            slideshow[j]->do_tags();

            return slideshow;
        }

};

void print_slideshow(const SlideShow &slideshow){

	for(auto &element: slideshow){
		cout << "ELEMENT" << endl;
		for(auto &tag: element->get_tags())
			cout << tag << endl;

	}


}

SlideShow hill_climb(pair<SlideShow, SlideShow> &slides){

        SlideShow working_cpy(slides.first);
        /*sort(working_cpy.begin(), working_cpy.end()-working_cpy.size()/2, [](auto &left, auto &right) { return left->number_tags() < right->number_tags(); });
        sort(working_cpy.end()-working_cpy.size()/2, working_cpy.end(), [](auto &left, auto &right) { return left->number_tags() > right->number_tags(); });
		*/
		std::random_device rd;
		std::mt19937 g(rd());
		shuffle(working_cpy.begin(), working_cpy.end(), g);

		auto cur_value = evaluation(working_cpy);
		std::uniform_int_distribution<> dis(0, working_cpy.size());
		std::uniform_int_distribution<> vert_dis(0, slides.second.size());
		for(int i = 0; i<working_cpy.size(); i++){

			auto l = dis(g);
			auto r = dis(g);
			if(l == r)
				r = (r+1)%working_cpy.size();

			Operator::swap_slides(working_cpy, l, r);
			auto new_val = evaluation(working_cpy);
			if(new_val > cur_value){
				cur_value = new_val;
				cout << "SWAP - New Value! " << cur_value << endl;
				continue;
			}
			Operator::swap_slides(working_cpy, l, r);


            auto vert_i = vert_dis(g);
            auto vert_j = vert_dis(g);

            Operator::swap_verticals(slides.second, vert_i, vert_j);
			new_val = evaluation(working_cpy);
			if(new_val > cur_value){
				cur_value = new_val;
				cout << "VERT - New Value! " << cur_value << endl;
				continue;
			}
            Operator::swap_verticals(slides.second, vert_i, vert_j);

		}

        return working_cpy;
}

inline bool acceptMove(int iteration, int currentScore, int NextScore){

    int delta = NextScore -currentScore;

    if(delta >0 ){
        return true;
    }

    else{
      return  ((double) rand() / (RAND_MAX)) < exp(delta/(maxIteration-iteration));
    }


}


SlideShow simulated_annealing(pair<SlideShow, SlideShow> &slides){

}

int main(){

		srand(time(nullptr));
        for(int i=3; i<4; i++){
                auto before = loadInput(i);
                cout << "Before: " << evaluation(before.first) << endl;
                auto after = hill_climb(before);
                cout << "After: " << evaluation(after) << endl;
        }
        return 0;
}
