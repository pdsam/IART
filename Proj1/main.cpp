#include <iostream>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <list>
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


pair<SlideShow, SlideShow> load_input(int input)
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
        "input/e_shiny_selfies.txt",
        "input/g_sheisse.txt",
	};

    file.open(files[input]);
    getline(file, line);
	map<string,int> simplifier;
	auto counter = 0;
	while (getline(file, line))
    {
        string orientation;
		int	number_tags;

        Image *image = new Image();
        istringstream iss(line);
        iss >> orientation;
		iss >> number_tags;

        image->orientation = (orientation == "H" ? H :  V);

        for (int i=0;i<number_tags;i++)
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

	random_device rd;
	shuffle(slideshow.begin(), slideshow.end(), mt19937(rd()));

	size_t i = 0;
	for_each(slideshow.begin(), slideshow.end(), [&i](auto s) {
				s->index=i++;
			});
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

void print_slideshow(const SlideShow &slideshow){

	cout << "START SLIDESHOW" << endl;
	for(auto &element: slideshow){
		cout << "ELEMENT" << endl;
		for(auto &tag: element->get_tags())
			cout << tag << endl;

	}
	cout << "END SLIDESHOW" << endl;

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

        SlideShow swap_verticals(SlideShow &slideshow, int i, int j, int v_i, int v_j) {
            if (!slideshow[i]->is_vertical() || !slideshow[j]->is_vertical()) {
                return slideshow;
            }

            swap(slideshow[i]->vert_images[v_i], slideshow[j]->vert_images[v_j]);
            slideshow[i]->do_tags();
            slideshow[j]->do_tags();

            return slideshow;
        }

};


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
    int sum = 0;

    if (i > 0) {
        sum += calculate_trasition(slideshow[i-1], slideshow[i]);
    }

    if (i < slideshow.size()-1) {
        sum += calculate_trasition(slideshow[i], slideshow[i+1]);
    }

    return sum;
}

bool accept_move_hill_climb(int unused, int unused2, int delta){

	return delta > 0;
}

inline bool accept_move_annealing(int iteration,int maxIteration , int delta){
    if(delta >0){
        return true;
    }
    else if(delta == 0){return false;}

    else{
    cout << "i: " << iteration << " maxIteration: " << maxIteration << " delta: " << delta << "\n";
      return  ((double) rand() / (RAND_MAX)) < exp(delta/(maxIteration/(double)iteration));
    }


}


SlideShow climb_with_heuristic(pair<SlideShow, SlideShow> &slides, function<bool(int, int, int)> accept_func){
    SlideShow working_cpy(slides.first);

	std::random_device rd;
	std::mt19937 g(rd());

	std::uniform_int_distribution<> dis(0, working_cpy.size()-1);
	std::uniform_int_distribution<> vert_dis(0, slides.second.size()-1);


	auto cur_value = evaluation(working_cpy);
	cout << working_cpy.size() << "\n";
	for(int i = 0; i<working_cpy.size(); i++){
		auto l = dis(g);
		auto r = dis(g);
		if(l == r)
			r = (r+1)%working_cpy.size();

		int before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
		Operator::swap_slides(working_cpy, l, r);
		int after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
		int diff = after - before;
		if(accept_func(i,working_cpy.size(),diff)){
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

		auto v_i = random()%2;
		auto v_j = random()%2;

		before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
		Operator::swap_verticals(slides.second, vert_i, vert_j, v_i, v_j);
		after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
		diff = after - before;
		if(accept_func(i,working_cpy.size(),diff)){
			cur_value += diff;
			cout << "VERT SWAP - New Value! " << cur_value << " " << i << endl;
			continue;
		}
		Operator::swap_verticals(slides.second, vert_i, vert_j, v_i, v_j);
	}

	return working_cpy;
}

long calculate_hash(const SlideShow &slideshow, int start_index){

	long res = 0;
	for(auto i = 0; i<16 && start_index+i<slideshow.size(); i++){
		long cur = 0;
		cur = reinterpret_cast<long>(&slideshow[i+start_index]);

		
		cur += reinterpret_cast<long>(slideshow[i+start_index]->vert_images[0]);
		if(slideshow[i+start_index]->is_vertical())
			cur += reinterpret_cast<long>(slideshow[i+start_index]->vert_images[1]);

		res += i*cur;
	}

	return res;
}

vector<long> block_hash(const SlideShow &slideshow){

	vector<long> result;
	result.reserve(slideshow.size()/16+1);
	for(auto i=0;i<slideshow.size();i+=16){
		result.push_back(calculate_hash(slideshow, i));
	}
	return result;
}

SlideShow tabu_search(pair<SlideShow, SlideShow> &slides){
    SlideShow working_cpy(slides.first);

	std::random_device rd;
	std::mt19937 g(rd());

	auto cur_hash = block_hash(working_cpy);
	list<pair<unsigned, vector<long>>> tabu_list;

	std::uniform_int_distribution<> dis(0, working_cpy.size()-1);
	std::uniform_int_distribution<> vert_dis(0, slides.second.size()-1);

	cout << working_cpy.size() << "\n";

	vector<function<SlideShow()>> operator_order;
	operator_order.reserve(working_cpy.size());

	auto cur_value = evaluation(working_cpy);
	int best_index = -1;
	for(int i = 0; i<working_cpy.size(); i++){

		tuple<unsigned, function<SlideShow()>, int, int> cur_op = make_tuple(0, nullptr, 0, 0);
		for(auto i=0;i<10;i++){

			bool horizontal_swap = (slides.second.size() == 0 ? true : (random() % 2 == 0));

			if(horizontal_swap){
				auto l = dis(g);
				auto r = dis(g);
				if(l == r)
					r = (r+1)%working_cpy.size();

				auto operation = bind(Operator::swap_slides, ref(working_cpy), l, r);
				int before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
				operation();
				int after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
				operation();

				if(get<0>(cur_op) < (after - before)+cur_value)
					cur_op = make_tuple(cur_value + (after - before), operation, l, r);
			}
			else{
				auto vert_i = vert_dis(g);
				auto vert_j = vert_dis(g);

				auto l = slides.second[vert_i]->index;
				auto r = slides.second[vert_j]->index;

				auto operation = bind(Operator::swap_verticals, ref(slides.second), vert_i, vert_j, random()%2, random()%2);
				auto before = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
				operation();
				auto after = calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
				operation();

				if(get<0>(cur_op) < (after - before)+cur_value)
					cur_op = make_tuple(cur_value + after - before, operation, l, r);
			}

		}

		if(get<1>(cur_op) == nullptr)
			continue;

		operator_order.push_back(get<1>(cur_op));
		operator_order[operator_order.size()-1]();
		auto cur_op_hash(cur_hash);
		auto left_block = get<2>(cur_op)/16;
		auto right_block = get<3>(cur_op)/16;

		cur_op_hash[left_block] = calculate_hash(working_cpy, left_block*16);
		if(left_block != right_block){
			cur_op_hash[right_block] = calculate_hash(working_cpy, right_block*16);
		}

		bool failed = false;
		for(auto &entry : tabu_list){
			if(entry.first != get<0>(cur_op))
				continue;

			if(cur_op_hash == cur_hash){
				operator_order[operator_order.size()-1]();
				operator_order.pop_back();
				failed=true;
				break;
			}
		}
		
		if(failed)
			continue;

		if(cur_value < get<0>(cur_op)){
			best_index = operator_order.size()-1;
			tabu_list.push_back(make_pair(cur_value, cur_hash));

			while(tabu_list.size() > 100)
				tabu_list.pop_front();

			cur_hash = cur_op_hash;
			cur_value = get<0>(cur_op);
			cout << "Found better one " << cur_value << " " << i << endl;
		}
	}

	for(auto it=operator_order.rbegin(); it!=operator_order.rend(); it++)
		(*it)();

	for(auto i=0;i<=best_index;i++)
		operator_order[i]();

	return working_cpy;
}

typedef vector<tuple<function<SlideShow()>, int, int>> Chromossome;
namespace Crossover{

	Chromossome one_point(const Chromossome &l, const Chromossome &r, unsigned point){

		Chromossome new_chromo;
		new_chromo.reserve(l.size());

		new_chromo.insert(new_chromo.begin(), l.begin(), l.begin()+point);
		new_chromo.insert(new_chromo.end(), r.begin()+point, r.end());

		return new_chromo;
	}

};


SlideShow genetic_algorithm(pair<SlideShow, SlideShow> &slides){

	SlideShow working_cpy(slides.first);

	std::random_device rd;
	std::mt19937 g(rd());
	std::uniform_int_distribution<> dis(0, working_cpy.size()-1);
	std::uniform_int_distribution<> vert_dis(0, slides.second.size()-1);

	const unsigned max_chromossome_size = 1000;
	std::uniform_int_distribution<> chrom_dis(0, max_chromossome_size);

	vector<Chromossome> current_gen;
	for(auto i=0;i<10;i++){
		current_gen.push_back(Chromossome());
		for(auto j=0;j<max_chromossome_size;j++){
			auto l = dis(g);
			auto r = dis(g);
			if(l == r)
				r = (r+1)%working_cpy.size();

			auto operation = bind(Operator::swap_slides, ref(working_cpy), l, r);
			current_gen[i].push_back(make_tuple(operation, l, r));

		}

	}
	std::uniform_int_distribution<> gen_dist(0, current_gen.size()-1);
	vector<Chromossome> next_gen;
	next_gen.reserve(current_gen.size()*2);
	vector<pair<int, int>> results;
	results.reserve(next_gen.size());

	int best_ans_value = 0;
	Chromossome best_ans;
	best_ans.reserve(max_chromossome_size);
	for(auto i=0;i<50;i++){
		next_gen.clear();
		cout << next_gen.size() << " " << next_gen.capacity() << endl;
		while(next_gen.size() != next_gen.capacity()){
			auto l = gen_dist(g);
			auto r = gen_dist(g);
			if(l == r)
				r = (r+1)%current_gen.size();

			next_gen.push_back(Crossover::one_point(current_gen[l], current_gen[r], chrom_dis(g)));
		}


		results.clear();
		for(auto j=0; j<next_gen.size(); j++){
			int after = 0;
			int before = 0;
			for(const auto &operation : next_gen[j]){
				auto l = get<1>(operation);
				auto r = get<2>(operation);
				before += calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
				get<0>(operation)();
				after += calc_around_slide(working_cpy, l) + calc_around_slide(working_cpy, r);
			}
			results.push_back(make_pair(after-before, j));

			for(auto it = next_gen[j].rbegin(); it!=next_gen[j].rend(); it++)
				get<0>(*it)();
		}

		sort(results.begin(), results.end(), [](auto &l, auto &r){
				return l.first > r.first;
				});
		for(auto j=0; j<current_gen.size();j++){
			auto &current = current_gen[j];
			current.clear();

			auto &replacer = next_gen[results[j].second];
			current.insert(current.begin(), replacer.begin(), replacer.end());
		}


		if(best_ans_value < results[0].first){
			best_ans_value = results[0].first;
			cout << "Found new best! Delta: " << best_ans_value << endl;
			best_ans.clear();
			auto &replacer = next_gen[results[0].second];
			best_ans.insert(best_ans.begin(), replacer.begin(), replacer.end());
		}

	}

	for(const auto &operation : best_ans)
		get<0>(operation)();

	return working_cpy;
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

        auto before = load_input(problem-1);
        switch(algo) {
            case 1:
                climb_with_heuristic(before, accept_move_hill_climb);
                break;
            case 2:
                climb_with_heuristic(before, accept_move_annealing);
                break;
            case 3:
                tabu_search(before);
                break;
            case 4:
		genetic_algorithm(before);
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
