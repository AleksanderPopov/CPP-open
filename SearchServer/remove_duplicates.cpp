#include "remove_duplicates.h"

 void RemoveDuplicates(SearchServer& search_server) {

   std::map<std::string, double>duplicate;
   std::map<std::set<std::string>, int> uniq_word_to_id;
   std::set<std::string> uniq_word_key;
   std::vector<int> remove_id = {};

   for (auto id : search_server) {
   
       duplicate = search_server.GetWordFrequencies(id);
       
       uniq_word_key = {};
       for (auto uniq_word : duplicate) {
           uniq_word_key.insert(uniq_word.first);
       }

       if (!uniq_word_to_id.count(uniq_word_key)) {
           uniq_word_to_id[uniq_word_key] = id;
       }
       else
       {
           remove_id.push_back(id);
       }
   }

     for (auto id : remove_id) {
         search_server.RemoveDocument(id);
         std::cout << "Found duplicate document id " << id << std::endl;
     }
};
