//IDENTIFIER  = 50EB44D3F029ED934858FFFCEAC3547C68768FC9#include <iostream>
#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <getopt.h>

using namespace std;

//NAME: EECS281 PROJECT1
//PROGRAMMER: YUZHOU CHEN
//DATA: 01/29/2024

struct DictionaryEntrance
{
    string word;
    size_t prev;
    bool discovered;
    DictionaryEntrance():word(""), prev(0), discovered(false) {};
    explicit DictionaryEntrance(string in_word):word(in_word), prev(0), discovered(false) {};
};

class Letterman
{
    private:
        string begin_word;
        string end_word;
        size_t begin_word_index;
        size_t end_word_index;
        char output_model;
        bool allow_change;
        bool allow_length;
        bool allow_swap;
        bool is_stack;
        string current_word;
        deque<size_t> search_container;
        //which will store the output list
        vector<size_t> output_vec;
        size_t dict_total_number;   
        vector<DictionaryEntrance> Dictionary;
        size_t num_words_found;
        // read dict, if illegal dict, return false
        bool readDict();
        // if cna not find the path, return false
        bool search();
        //output the solution
        void output();
        //compare if two strings are same
        bool same(const string &A, const string &B);
        //compare if two strings similar
        bool compare(const string &check_word, const string &object_word);
        //we know two strings are similar, output the difference
        pair<char,size_t> different(const string &A, const string &B);
        

    public:
        //ctor
        Letterman();
        //if begin = end, return false
        bool get_options(int argc, char** argv);
        //main function
        bool main_manu();
};

int main(int argc, char** argv) 
{
    std::ios_base::sync_with_stdio(false);
    Letterman L; 
    if (!L.get_options(argc, argv))
    {
        return 0;
    }
    
    if (L.main_manu())
    {
        return 0;
    };
    return 0;
};

bool Letterman::main_manu()
{
    if(!readDict())
    {
        return true;
    }
    if (!search())
    {
        return true;
    }
    output();
    return true;
}

Letterman::Letterman()
{
    begin_word_index = 0;
    end_word_index = 0;
    num_words_found = 0;
    begin_word = "";
    end_word = "";
    output_model = '\0';
    is_stack = true;
    allow_change = false;
    allow_length = false;
    allow_swap = false;
    dict_total_number = 0;
};

bool Letterman::get_options(int argc, char** argv)
{
    int option_index = 0, option = 0;
    bool stack_or_queue = false, end_word_exist = false, begin_word_exist = false, output_exist = false;
    struct option longOpts[] = {  {"stack", no_argument, nullptr, 's'},
                                {"queue", no_argument, nullptr, 'q'},
                                {"length", no_argument, nullptr, 'l'},
                                {"change", no_argument, nullptr, 'c'},
                                {"swap", no_argument, nullptr, 'p'},
                                {"output", required_argument, nullptr, 'o'},
                                {"begin", required_argument, nullptr, 'b'},
                                {"end", required_argument, nullptr, 'e'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, '\0'} };

    while ((option = getopt_long(argc, argv, "sqlcpo:b:e:h", longOpts, &option_index)) != -1)
    {
        switch (option) 
        {
            case 's':
                if(!stack_or_queue)
                {
                    is_stack = true;
                    stack_or_queue = true;
                }
                else
                {
                    cerr << "You can not choose both stack and queue" << "\n";
                    exit(1);
                }
                break;

            case 'q':
                if(!stack_or_queue)
                {
                    is_stack = false;
                    stack_or_queue = true;
                }
                else
                {
                    cerr << "You can not choose both stack and queue" << "\n";
                    exit(1);
                }
                break;

            case 'l':
                allow_length = true;
                break;

            case 'c':
                allow_change = true;
                break;

            case 'p':
                allow_swap = true;
                break;

            case 'b':
                begin_word = optarg;
                begin_word_exist = true;
                break;

            case 'e':
                end_word = optarg;
                end_word_exist = true;
                break;

            case 'o':
                    output_exist = true;
                    output_model = *optarg;
                break;
            case 'h':
                cerr << "This program reads in a dictionary from a file.\n"
                    << "It runs takes in an end word as an initial argument\n"
                    << "and generates a path from the beginning word to the end \n"
                    << "word according to the provided arguemnts."
                    << "Usage: \'./letter\n\t[--stack | -s]\n"
                    << "\t[--queue | -q]\n"
                    << "\t[--change | -c]\n"
                    << "\t[--swap | -p]\n"
                    << "\t[--length | -l]\n"
                    << "\t[--output | -o] (W|M)\n"
                    << "\t[--begin | -b] <word>\n"
                    << "\t[--end | -e] <word>\n"
                    << "\t[--help | -h]\n"
                    << "\t<Dictionary File>\'" 
                    << "\n";
                exit(0);

            default:
                cerr << "Unknown command line option" << endl;
                exit(1);
        }
    }

    if (!stack_or_queue)
    {
        cerr << "Must specify one of stack or queue" << "\n";
        exit(1);
    }
    if (!begin_word_exist||!end_word_exist)
    {
        cerr << "You must choose both begin_word and end_word" << "\n";
        exit(1); 
    }
    if (begin_word.size() != end_word.size() && !allow_length)
    {
        cerr << "The first and last words must have the same length when" 
                << "length mode is off " << "\n";
        exit(1);
    }

    if(!output_exist)
    {
        output_model = 'W';
    }
    else if(output_model != 'M' && output_model != 'W')
    {
        cerr << "ouput_model should be 'W' or 'M', but you chose " 
             << output_model << "\n";
        exit(1);
    }

    if (!allow_change && !allow_length && !allow_swap)
    {
        cerr << "Please make at least one option among -l, -p or -c" << "\n";
        exit(1);
    }
    if (begin_word.empty() || end_word.empty())
    {
        cerr << "begin_word and end_word can not be empty" << "\n";
        exit(1);
    }
    // if beignning word == ending word
    if (same(begin_word, end_word))
    {
        cout << "Words in morph: 1" << "\n" << begin_word << "\n";
        return false;
    }

    if (begin_word.size() != end_word.size() && !allow_length)
    {
        cerr << "begin_word and end_word have different size, but their " 
            << "length can not be modified" << "\n";
        exit(1);
    }
    return true;
};

pair<char,size_t> Letterman::different(const string &A, const string &B)
{
    size_t A_size = A.size(), B_size = B.size();

    if (A_size != B_size)
    {
        char type;
        size_t max_size;
        if(A_size < B_size)
        {
            type = 'i';
            max_size = B_size;
        }
        else
        {
            type = 'd';
            max_size = A_size;
        }
        size_t location = (max_size - 1);
        for (size_t i = 0; i < max_size; i++)
        {
            if (A[i] !=  B[i])
            {
                location = i;
                break;
            }
        }
        return make_pair(type, location);
    }
    else
    {
        for (size_t i = 0; i < A.size(); i++)
        {
            if (A[i] !=  B[i])
            {
                if (i == (A.size() - 1))
                {
                    return make_pair('c', i);
                }
                else 
                {
                    if(A[i + 1] == B[i + 1])
                    {
                        return make_pair('c', i);
                    }
                    else
                    {
                        return make_pair('s', i);
                    }
                }
            }
        }
    }
    return make_pair('\0', 0);
};


void Letterman::output()
{
    // cout << "Words in morph: " << removed_word.size() << "\n" 
    //     << Dictionary[removed_word[0]].word << "\n";
    std::cout << "Words in morph: ";

    //creat a reversed output vector
    output_vec.push_back(end_word_index);
    size_t index = Dictionary[end_word_index].prev;

    while(index != begin_word_index)
    {
        //cout << index << "\n";
        output_vec.push_back(index);
        index = Dictionary[index].prev;
    }
    output_vec.push_back(begin_word_index);
    //output_vec.push_back(index);

    //revervse it
    reverse(output_vec.begin(),output_vec.end());

    //start output
    size_t output_size = output_vec.size();
    std::cout << output_size << "\n";
    //word model////////////////////////////////////////////////////////////////
    if(output_model == 'W')
    {
        for (size_t i = 0; i < output_size; i++)
        {
            std::cout << Dictionary[output_vec[i]].word << "\n";
        }

    }
    //modification model////////////////////////////////////////////////////////
    else
    {
        cout << Dictionary[begin_word_index].word << "\n";
        pair<char,size_t> temp;
        for (size_t i = 1; i < output_size; i++)
        {
            temp = different(Dictionary[output_vec[i - 1]].word, Dictionary[output_vec[i]].word);
            switch(temp.first)
            {
                case 'i':
                    cout << "i," << temp.second << "," << Dictionary[output_vec[i]].word[temp.second] << endl;
                    break;
                case 'c':
                    cout << "c," << temp.second << "," << Dictionary[output_vec[i]].word[temp.second] << endl;
                    break;
                case 's':
                    cout << "s," << temp.second << endl;
                    break;
                case 'd':
                    cout << "d," << temp.second << endl;
                    break;
                default:
                    break;
            }
        }
    }
}

bool Letterman::same(const string &A, const string &B)
{
    if (A.size() == B.size())
    {
        for (size_t i = 0; i < A.size(); i++)
            if (A[i] != B[i]){return false;}
    }
    else{return false;}
    return true;
};


bool Letterman::search()
{
///////////////////////Prepare for Searching////////////////////////////////////////////////////////
    size_t current_index = 0;
    bool found_begin = false, found_end = false;

     //firstly, find the begin_word
    for (size_t i = 0; i < dict_total_number; i++)
    {
        if(same(Dictionary[i].word, begin_word))
        {
            found_begin = true;
            //current_word = Dictionary[i].word;
            Dictionary[i].discovered = true;
            num_words_found++;
            //removed_word.push_back(i);
            search_container.push_back(i);
            current_index = i;
            break;
        }
    }
    
    //if find end_word, mark it
    for (size_t i = 0; i < dict_total_number; i++)
    {
        if(same(Dictionary[i].word, end_word))
        {
            found_end = true;
            end_word_index = i;
            break;
        }
    }

    begin_word_index = current_index;
    //if no begin_word found
    if (!found_begin || !found_end)
    {
        cerr << "No soluiton, cannot find begin or end word." << "\n";
        exit(1);
    }

    bool found_path = false;
////////////////////////Searching Start Here////////////////////////////////////////////////////////
    while(!found_path)
    {
        //step0: initialization
        current_word = Dictionary[current_index].word;
        //step0.5: test if container can't be pop, meaning no path found
        if (search_container.empty())
        {
            cout << "No solution, " << num_words_found << " words discovered." << "\n";
            return false;
        }
        if (is_stack){search_container.pop_back();}
        else{search_container.pop_front();}
        //step1: find every similar words and push_back
        for (size_t k = 0; k < dict_total_number; k++)
        {
            if (!Dictionary[k].discovered)
            {
                if (compare(Dictionary[k].word, current_word))
                {
                    search_container.push_back(k);
                    Dictionary[k].discovered = true;
                    num_words_found++;
                    Dictionary[k].prev = current_index;

                    //step1.5: find if current = end
                    if (k == end_word_index)
                    {
                        found_path = true;
                        return true;
                    }
                }
            }   
        }
        //step 2: pop next word to removed_word 
        if (search_container.empty())
        {
            cout << "No solution, " << num_words_found << " words discovered." << "\n";
            return false;
        }

        if (is_stack){current_index = search_container.back();}
        else{ current_index = search_container.front();}
    }
    return false;
};


////////////////////////////////////////////////////////////////////////////////
bool Letterman::compare(const string &check_word, const string &object_word)
{
    bool is_similar = false;
    int different_amount = 0;
    int length_gap = int(check_word.size()) - int(object_word.size());
    //check if inserting or deleting
    if (length_gap != 0)
    {
        if (allow_length == false)
        {
            return is_similar;
        }

        if (length_gap == 1 || length_gap == -1)
        {
            size_t i = 0, j = 0;

            while (i < check_word.size() && j < object_word.size())
            {
                if (check_word[i] == object_word[j])
                {
                    i++;
                    j++;
                }
                else
                {
                    if (length_gap == 1)
                    {
                        i++;
                    }
                    else
                    {
                        j++;
                    }
                    different_amount ++;
                }

                if (different_amount > 1)
                {
                    //cout << "expect to be a length, but not length" << "\n";
                    return is_similar;
                }
            }
            is_similar = true;
            //cout << "length" << "\n";
            return is_similar;
        }
        else
        {
            //cout << length_gap << "\n";
            return is_similar;
        }
    }

    //check total different letters
    size_t i = 0;
    ///////////////////////////////////////////////////////////////////////////
    //may be swap_pos could be store in the struct
    size_t swap_position = 0;
    while (i <= check_word.size() - 1)
    {
        if (check_word[i] != object_word[i])
        {
            different_amount ++;
            //record potantial swap_pos, but only once
            if(i > 0)
            {
                if (check_word[i - 1] != object_word[i - 1] && 
                    different_amount == 2)
                {
                    swap_position = i;
                }
            }
        }

        i++;

        //if more than 2 letters are different
        if (different_amount > 2)
        {
            return is_similar;
        }
    }
    //cout << different_amount << "\n";

    //check if same
    if (different_amount == 0)
    {
        is_similar = true;
        //cout << "same" << "\n";
    }
    //check if change
    if (different_amount == 1 && allow_change == true)
    {
        is_similar = true;
        //cout << "change" << "\n";
    }

    //check if swap
    if (different_amount == 2 && allow_swap == true)
    {
        if (check_word[swap_position - 1] == object_word[swap_position] && 
            check_word[swap_position] == object_word[swap_position - 1])
        {
            is_similar = true;
        }
        //cout << "swap" << "\n";
    }
    return is_similar;
};


////////////////////////////////////////////////////////////////////////////////
bool Letterman::readDict()
{
    //test dictionary type: S/C
    char type;
    cin >> type;
    //cout << type << "\n";
    if (type != 'S' && type != 'C')
    {
        cout << "Dictionary type invalid" << endl;
        return false;
    }
    //test capacity
    size_t capacity;
    cin >> capacity;
    //cout << capacity << "\n";
    //test if next line is comment
    char slash;
    string comment;
    cin >> slash;
    while (slash == '/')
    {
        cin.putback(slash);
        getline(cin, comment);
        cin >> slash;
        //cout << comment << "\n";
    }
    cin.putback(slash);
    //start collecting the dictionary entrance
    string dict_word;
    size_t index = 0;
    DictionaryEntrance insert_this;
    size_t cin_count = 0;

    //Dictionary.reserve(capacity);
    //check different dictinary type
    if (type == 'S')
    {   
        Dictionary.reserve(capacity);
        //cout << Dictionary.size() <<"//////////////////////" << dict_total_number << "/////////////" << endl;
        while(cin_count < capacity)
        {
            cin >> dict_word;
            cin_count++;
            if (allow_length || dict_word.size() == begin_word.size())
            {
                insert_this.word = dict_word;
                //cout << dict_word << "\n";
                Dictionary.push_back(insert_this);
                //index++;
            }
        }
        dict_total_number = Dictionary.size();
    }
    //complex dictionary
    else if (type == 'C')
    {
        while (cin_count < capacity)
        {
            cin >> dict_word;
            cin_count++;
            //cout << dict_word << "\n";
           
            for (size_t i = 0; i < dict_word.size(); i++)
            {
                if (dict_word[i] == '!')
                {
                    if(allow_length || dict_word.size() == begin_word.size() + 1)
                    {
                        //push original one
                        insert_this.word = (dict_word.substr(0,i) + dict_word.substr(i + 1, dict_word.size() - 1));
                        Dictionary.push_back(insert_this);
                        //push modificated
                        insert_this.word = (dict_word.substr(0, i - 2) + dict_word[i - 1] + dict_word[i - 2] + dict_word.substr(i + 1, dict_word.size() - 1));
                        Dictionary.push_back(insert_this);
                        break;
                    }
                }

                else if (dict_word[i] == '&')
                {
                    if(allow_length || dict_word.size() == begin_word.size() + 1)
                    {
                        //push original one
                        insert_this.word = dict_word.substr(0, dict_word.size() - 1);
                        Dictionary.push_back(insert_this);
                        //push modificated
                        reverse(insert_this.word.begin(), insert_this.word.end());
                        Dictionary.push_back(insert_this);
                    }
                }

                else if (dict_word[i] == '?')
                {
                    if(allow_length || dict_word.size() == begin_word.size() + 1)
                    {
                        //push original one
                        insert_this.word = (dict_word.substr(0,i) + 
                        dict_word.substr(i + 1, dict_word.size() - 1));
                        Dictionary.push_back(insert_this);
                    }
                    if(allow_length || dict_word.size() == begin_word.size())
                    {
                        //push modificated
                        insert_this.word = (dict_word.substr(0, i - 1) +
                        dict_word[i - 1] + dict_word[i - 1] +
                        dict_word.substr(i + 1, dict_word.size() - 1));
                        Dictionary.push_back(insert_this);
                        break;
                    }
                }

                else if (dict_word[i] == '[')
                {
                        size_t temp_index = i;
                        //check where is the ']'

                        while (dict_word[i] != ']')
                        {
                            i++;
                            //check if there is no ']'
                            if (i > (dict_word.size() - 1))
                            {
                                cout << "invalid complex word, no ] found " << endl;
                                return false;
                            }
                        }
                        size_t temp_position = i;
                        if(allow_length || dict_word.size() == begin_word.size() + (temp_position - temp_index))
                        { 

                            for (size_t j = temp_index + 1; j < i; j++)
                            {
                                //push original one
                                insert_this.word = (dict_word.substr(0, temp_index) + 
                                dict_word[j] + dict_word.substr(i+1,dict_word.size()));
                                Dictionary.push_back(insert_this);
                            }
                            break;
                        }
                }

                else
                {
                    if (i == dict_word.size() - 1)
                    {
                        insert_this.word = dict_word;
                        Dictionary.push_back(insert_this);
                    }
                }
            }
            index++;
            dict_total_number = Dictionary.size();
        }
    }
    // for (int i = 0 ; i < dict_total_number; i++)
    // {
    //     cout << Dictionary[i].word << "\n";
    // }
    return true;
};