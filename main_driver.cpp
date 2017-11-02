/*#include <iostream>
#include <string>
#include <ctype.h>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <iomanip>
#include <stack>
#include <stdio.h>
#include <cstdlib>
#include <sstream>
*/
//#include "webcrawler.h"
#include "grnn.h"


//using namespace std;

int range1 = 0; // -1.0 to -0.5
int range2 = 0; // -0.5 to 0.0
int range3 = 0; // 0.0 to 0.5
int range4 = 0; // 0.5 to 1.0
int unknown_range = 0;

//stack that stores URLs
stack<string> url_stack;
//stack that stores depths of URLs
stack<int> depth_stack;

//TODO: SHOULD THIS BE DYNAMICALLY ALLOCATED?
double weights[NUM_FEATURES]; //TODO: IS THIS THE RIGHT SIZE?

string filepath;
string filename = "weights.txt";
int depth_limit = -1;
int depth = 0;
int i = 0;

class WebCrawler {
  public:
    WebCrawler();
    int find_children(string filename, int depth, stack<string>& stack1, stack<int>& stack2, int i, bool leaf);
    void char_extractor(string filename, int k);
    int webcrawler(stack<string>& url_stack, stack<int>& depth_stack, int depth_limit, int& i);
    int init_trng_set(Data_Point trng_set[]);
    void init_trng_point(Data_Point trng_set, string filename);
    int init_weights(double weights[], string filename);
};



int main() {

  //prompting input
  cout << "Please input a root URL: ";
  cin >> filepath;

  //checking user input
  while (depth_limit < 0) {
      cout << "Please input a depth limit >= 0:  ";
      cin >> depth_limit;
  }

  //push root filepath onto stack
  url_stack.push(filepath);

  //push root depth onto stack
  depth_stack.push(0);

  //Read in trng_set
  Data_Point trng_set[635]; //595 instances plus room for 40 additional instances
  WebCrawler web = WebCrawler();
  web.init_trng_set(trng_set);

  //TODO: Read in weights into a dynamically allocated array for grnn_classify function
  web.init_weights(weights, filename);

  //stopping conditions
  while (range1 < 10 || range2 < 10 || range3 < 10 || range4 < 10) {

    //exhausted searching for URLs to the depth limit
    if (url_stack.empty()) {
      break;
    }
    //Provide webcrawler with a URL, which prints out
    //feature vectors to a file one by one
    int j = web.webcrawler(url_stack, depth_stack, depth_limit, i);

    //Create Data Point instance based on output file produced by webcrawler in unigrams directory
    Data_Point data;

    web.init_trng_point(data, "output_" + to_string(j - 1) + "_unigrams.txt");

    double classification = data.grnn_classify(trng_set, 0.1237, weights); //TODO: IS THIS FUNCTION CORRECT?

    if ((classification > -1 && classification < -0.5) || (classification == -1) || (classification == -0.5)) {
      range1++;
    }
    else if ((classification > -0.5 && classification < 0) || (classification == 0) || (classification == -0.5)) {
      range2++;
    }
    else if ((classification > 0 && classification < 0.5) || (classification == 0) || (classification == 0.5)) {
      range3++;
    }
    else if ((classification > 0.5 && classification < 1) || (classification == 0.5) || (classification == 1)) {
      range4++;
    }
    else {
      unknown_range++;
    }
  }

return 0;

}

WebCrawler::WebCrawler() {  // Instantiation
}

void WebCrawler::char_extractor(string filename, int k) {
    char c;
    double unigram_count[95] = {0};
    double total_unigrams = 0;
  //  string feature_vectors = "feature_vectors.txt";

    ifstream in_text("html_source/" + filename);

    while (in_text >> noskipws >> c) {
        if (isprint(c)) {
            int i = c - 32;
            unigram_count[i]++;
            total_unigrams++;
        }
    }

    in_text.close();

    // Create and write to new text file containing relative frequencies

    string new_filename = filename.erase(filename.length() - 4, 4) + "_unigrams.txt";
    ofstream out_text("unigrams/" + new_filename);

    //ofstream out_text(feature_vectors, ofstream::app);
    out_text << k;

    for (int i = 0; i < 95; i++) {
        //char a = i + 32;
        // IMPROVEMENT: NORMALIZE UNIGRAM FREQUENCY BY TOTAL UNIGRAMS
        // out_text << a << ": " << unigram_count[i] / total_unigrams << "\n";
        out_text << " " << (unigram_count[i] / total_unigrams);
    }

    out_text.close();
}



int WebCrawler::find_children(string filename_str, int depth, stack<string>& stack1, stack<int>& stack2, int i, bool leaf) {

    char javaCall[100] = "";
    char url[100] = "";
    string new_url_str, unnecessary = "";
    int num_of_links = 0;
    string j = to_string(i);
    char *k = new char[j.size()+1];
    strcpy(k, j.c_str());

    char *filename = new char[filename_str.size()+1];
    strcpy(filename, filename_str.c_str());

    // Build call to getWebPage

    strcat(javaCall, "java getWebPage ");
    strcat(javaCall, filename);
    strcat(javaCall, " > html_source/output_");
    strcat(javaCall, k);
    strcat(javaCall, ".txt");
    system(javaCall);
    ifstream webpage("html_source/output_" + j + ".txt");

    while (!webpage.eof() && !leaf) {
        webpage >> setw(99) >> url;
        if (strncmp("href", url, 4) == 0) {
            // cout << url << endl;
            string url_str(url);
            //TODO: FIGURE OUT IF URL HAS NOT ALREADY BEEN SEEN --> FIX IF THERE IS TIME

            //Remove href=" from the front of the URL and remove " from the end of the URL:
            //Use quotation marks as delimiters

            stringstream ss(url_str);


            getline(ss, unnecessary, '"');
            getline(ss, new_url_str, '"');

            //IMPROVEMENT: prevents any URLS that do not begin with "http" from being pushed onto the stack

            if ((new_url_str[0] != '#') && (new_url_str.substr(new_url_str.length() - 3, new_url_str.length()) != "pdf") && (new_url_str.substr(0, 4) == "http") && strncmp(url, filename, sizeof(url)/sizeof(url[0]))) {
                //push URL onto stack
                stack1.push(new_url_str);
                cout << "Stack1: " << stack1.top() << endl;

                //store associated depth of URL into stack as (depth + 1)
                stack2.push(depth + 1);
                cout << "Stack2: " << stack2.top() << endl;
                num_of_links++;
            }
        }
    }

  //  cout << "depth " << (i + 1) << " done" << endl;

    webpage.close();

    return num_of_links;
}

int WebCrawler::webcrawler(stack<string>& url_stack, stack<int>& depth_stack, int depth_limit, int& i) {

  string filepath = "";
  int depth = 0;
  /*
  int i = 0;
  int depth_limit = -1;

  //stack that stores URLs
  stack<string> url_stack;

  //stack that stores depths of URLs
  stack<int> depth_stack;

  //prompting input
  cout << "Please input a root URL: ";
  cin >> filepath;


  //checking user input
  while (depth_limit < 0) {
      cout << "Please input a depth limit >= 0:  ";
      cin >> depth_limit;
  }

  //push root filepath onto stack
  url_stack.push(filepath);

  //push root depth onto stack
  depth_stack.push(0);
*/

  //IMPROVEMENT: WE ARE USING DEPTH-LIMITED SEARCH INSTEAD OF ITERATIVE DEEPENING SEARCH
//  while (!url_stack.empty()) {

      //pop url from queue
      filepath = url_stack.top();
      cout << "filepath being expanded is " << filepath << endl;
      url_stack.pop();


      //set depth to depth associated with url by checking array --> necessary for backtracking up the tree of URLs
      depth = depth_stack.top();
      depth_stack.pop();
      cout << "the depth of the filepath being expanded is " << depth << endl;


      //Find children of URL and store them into the stack. Also store their associated depths into a stack. Saves HTML of expanded URL in a .txt file
      find_children(filepath, depth, url_stack, depth_stack, i, depth >= depth_limit);

      char_extractor("output_" + to_string(i) + ".txt", i);
      i++;

      return i;
}


int WebCrawler::init_weights(double weights[], string filename) {
  ifstream input(filename);
  string line;
  int j = 0;
  while (getline(input, line)) {
      weights[j] = stod(line); //convert string to double
      j++;
  }
  return j;
}


int WebCrawler::init_trng_set(Data_Point trng_set[]) {
        ifstream input("our_dataset.txt");
        string line;
        int j = 0;
        double magnitude;
        while (getline(input, line)) {
            trng_set[j++] = Data_Point(line);
            magnitude = 0;
            for (int k = 0; k < 95; k++) {
                magnitude += pow(trng_set[j - 1].feat_vecs[k], 2);
            }
            if (magnitude != 0) {
                for (int k = 0; k < 95; k++) {
                    trng_set[j - 1].feat_vecs[k] /= sqrt(magnitude);
                }
            }
        }
        return j;
    }


    void WebCrawler::init_trng_point(Data_Point trng_set, string filename) {
            ifstream input("unigrams/" + filename);
            string line;
            double magnitude;
            while (getline(input, line)) {
                trng_set = Data_Point(line, 0);
                magnitude = 0;
                for (int k = 0; k < 95; k++) {
                    magnitude += pow(trng_set.feat_vecs[k], 2);
                }
                if (magnitude != 0) {
                    for (int k = 0; k < 95; k++) {
                        trng_set.feat_vecs[k] /= sqrt(magnitude);
                    }
                }
            }
        }