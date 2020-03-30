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
        size_t index;

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
                slideshow[i]->index = i;
                slideshow[j]->index = j;
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

int calculate_trasition(Slide* left, Slide* right) {
    auto left_tags = left->get_tags();
    auto right_tags = right->get_tags();

    const auto small_to_big = (left_tags.size() > right_tags.size() ? make_pair(right_tags,left_tags) : make_pair(left_tags, right_tags));
    int common_tags = 0;

    for(const auto &element : get<0>(small_to_big)){
            if(get<1>(small_to_big).find(element) != get<1>(small_to_big).end())
                    common_tags +=1;
    }

    return min(left->number_tags()-common_tags, min(common_tags, right->number_tags()-common_tags));
}

int calc_around_slide(SlideShow& slideshow, int i) {
    int sum{};

    if (i > 0) {
        sum += calculate_trasition(slideshow[i-1], slideshow[i]);
    }

    if (i < slideshow.size()-1) {
        sum += calculate_trasition(slideshow[i], slideshow[i+1]);
    }

    return sum;
}

SlideShow hill_climb(pair<SlideShow, SlideShow> &slides){

        SlideShow working_cpy(slides.first);

		std::random_device rd;
		std::mt19937 g(rd());

		shuffle(working_cpy.begin(), working_cpy.end(), g);

		std::uniform_int_distribution<> dis(0, working_cpy.size()-1);
		std::uniform_int_distribution<> vert_dis(0, slides.second.size()-1);

        size_t i = 0;
        for_each(working_cpy.begin(), working_cpy.end(), [&i](auto s) {
                    s->index=i++;
                });

		auto cur_value = evaluation(working_cpy);
		for(int i = 0; i<working_cpy.size(); i++){
			auto l = dis(g);
			auto r = dis(g);
			if(l == r)
				r = (r+1)%working_cpy.size();

            int before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
			Operator::swap_slides(working_cpy, l, r);
            int after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
            int diff = after - before;
			if(diff > 0){
				cur_value += diff;
				cout << "     SWAP - New Value! " << cur_value << " " << i << endl;
				continue;
			}
			Operator::swap_slides(working_cpy, l, r);

			if(slides.second.size() == 0)
				continue;


            auto vert_i = vert_dis(g);
            auto vert_j = vert_dis(g);

            l = slides.second[vert_i]->index;
            r = slides.second[vert_j]->index;

            before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
            Operator::swap_verticals(slides.second, vert_i, vert_j);
            after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
            diff = after - before;
			if(diff > 0){
				cur_value += diff;
				cout << "VERT SWAP - New Value! " << cur_value << " " << i << endl;
				continue;
			}
            Operator::swap_verticals(slides.second, vert_i, vert_j);
		}

        return working_cpy;
}

namespace TabuTools {
    struct Tabu {
        vector<size_t> block_hashes;
    };

    const int hash_block_size = 1000;
    const int max_tabu_limit = 1000000;

    size_t calc_block_hash(SlideShow& slideshow, int block) {

        size_t hash = 0;
        int block_size = min((int)slideshow.size()-block*hash_block_size, hash_block_size);

        auto hasher = std::hash<Image*>();
        for (int j = 0; j < block_size; ++j) {
            int index = block_size*block + j;
            Slide* slide = slideshow[index];

            hash ^= hasher(slide->image);

            if (slide->is_vertical()){
                hash ^= hasher(slide->vert_images[1]);
            }
        }

        return hash;
    }

    void add_tabu(vector<Tabu>& tabus, SlideShow& slideshow) {

        if (tabus.size() == max_tabu_limit) {
            tabus.erase(tabus.begin());
        }

        Tabu tabu;
        int num_blocks = ceil((float) slideshow.size() / hash_block_size);
        for (int i = 0; i < num_blocks; ++i) {
            tabu.block_hashes.push_back(calc_block_hash(slideshow, i));
        }

        tabus.push_back(move(tabu));
    }

    bool check_tabu(vector<Tabu>& tabus, SlideShow& slideshow, int i, int j) {
        for (const Tabu& t: tabus) {

            int lBlock = i/hash_block_size;
            int rBlock = j/hash_block_size;

            auto lHash = calc_block_hash(slideshow, lBlock);
            auto rHash = calc_block_hash(slideshow, rBlock);

            if (lHash == t.block_hashes[lBlock] && rHash == t.block_hashes[rBlock]) {
                return true;
            }
        }

        return false;
    }
}

void deep_copy(SlideShow& src, SlideShow& dest) {
    for_each(src.begin(), src.end(), [&dest](auto slide){
                Slide* n_slide = new Slide();
                n_slide->image = slide->image;
                if (slide ->is_vertical()) {
                    n_slide->vert_images[1] = slide->vert_images[1];
                }
                n_slide->do_tags();
                dest.push_back(n_slide);
            });
}

SlideShow tabu_search(pair<SlideShow, SlideShow> &slides) {

    SlideShow working_cpy;

    deep_copy(slides.first, working_cpy);

    vector<TabuTools::Tabu> tabu_list;
    
    std::random_device rd;
    std::mt19937 g(rd());

    //shuffle(working_cpy.begin(), working_cpy.end(), g);

    std::uniform_int_distribution<> dis(0, working_cpy.size()-1);

    SlideShow best = working_cpy;
    int current_val = evaluation(best);
    int working_val = current_val;
    for(int i = 0; i < working_cpy.size(); ++i) {
        int l;
        int r;
        int before;
        int after;
        while(true) {
			l = dis(g);
			r = dis(g);
			if(l == r)
				r = (r+1)%working_cpy.size();

            before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
            Operator::swap_slides(working_cpy, l, r);
            after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);

            if (TabuTools::check_tabu(tabu_list, working_cpy, l, r)) {
                Operator::swap_slides(working_cpy, l, r);
            } else {
                break;
            }
        }

        Operator::swap_slides(working_cpy, l, r);
        TabuTools::add_tabu(tabu_list, working_cpy);
        Operator::swap_slides(working_cpy, l, r);

        int diff = after - before;
        working_val += diff;
        printf("%d\n", working_val);
        if (working_val > current_val) {
            current_val = working_val;
            best = working_cpy;
            printf("NEW VALUE! %d %d\n", current_val, i);
        }
    }

    return best;
}

int main(){
        cout << "Welcome to algoritmator 3000." << endl;

        static const string problems[] = {
            "1 : example - Simple problem.",
            "2 : lovely_landscapes - Only horizontal pictures.",
            "3 : memorable_moments - Mixed vertical and horizontal.",
            "4 : pet_pictures - Mixed but bigger.",
            "5 : shiny_selfies - Only vertical pictures."};

        cout << "\nChoose a problem:\n"; 

        for (const string& s: problems) {
            cout << "\t" << s << endl;
        }

        int problem;
        cout << "\nChoice: ";
        cin >> problem;

        const string algorithms[] = {
            "1 - Hill climb", 
            "2 - Simulated Annealing", 
            "3 - Tabu Search", 
            "4 - Genetic Algorithm"};

        cout << "\nChoose an algorithm:\n";
        for (const string& s: algorithms) {
            cout << "\t" << s << endl;
        }

        int algo;
        cout << "\nChoice: ";
        cin >> algo;

        auto before = loadInput(problem-1);
        switch(algo) {
            case 1:
                hill_climb(before);
                break;
            case 2:
                //annealing
                break;
            case 3:
                tabu_search(before);
                break;
            case 4:
                //genetic
                break;
            default:
                cout << "Invalid choice." << endl;
        }

        /*
		srand(time(nullptr));
        for(int i=3; i<4; i++){
                auto before = loadInput(i);
                cout << "Before: " << evaluation(before.first) << endl;
                auto after = tabu_search(before);
                cout << "After: " << evaluation(after) << endl;
        }*/
        return 0;
}
