//
// Created by זוהר on 17/08/2021.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORDS_IN_SENTENCE_GENERATION 20
#define MAX_WORD_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define VALIDATE(e) if(!(e)){exit (EXIT_FAILURE);}
#define MES1 "Usage: wrong number of arguments.\n"
#define MES2 "Error: file does not exist/ access not allowed."

/************ WORD STRUCT ************/
typedef struct WordStruct {
    char *word;
    struct WordProbability *prob_list;
    int num;
    int is_period;
    int diff_words;
} WordStruct;
/*************************************/

/************ WORD PROBABILITY ************/
typedef struct WordProbability {
    struct WordStruct *word_struct_ptr;
    int num_of_occ;
} WordProbability;
/*************************************/

/************ LINKED LIST ************/
typedef struct Node {
    WordStruct *data;
    struct Node *next;
} Node;

typedef struct LinkList {
    Node *first;
    Node *last;
    int size;
} LinkList;

/**
 * Add data to new node at the end of the given link list.
 * @param link_list Link list to add data to
 * @param data pointer to dynamically allocated data
 * @return 0 on success, 1 otherwise
 */
int add (LinkList *link_list, WordStruct *data)
{
  Node *new_node = malloc (sizeof (Node));
  if (new_node == NULL)
    {
      return 1;
    }
  *new_node = (Node) {data, NULL};

  if (link_list->first == NULL)
    {
      link_list->first = new_node;
      link_list->last = new_node;
    }
  else
    {
      link_list->last->next = new_node;
      link_list->last = new_node;
    }

  link_list->size++;
  return 0;
}
/*************************************/

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number (int max_number)
{

  return rand () % max_number;
}

/**
 * Choose randomly the next word from the given dictionary, drawn uniformly.
 * The function won't return a word that end's in full stop '.' (Nekuda).
 * @param dictionary Dictionary to choose a word from
 * @return WordStruct of the chosen word
 */
WordStruct *get_first_random_word (LinkList *dictionary)
{
  int end_word = true;
  struct Node *word_ptr;
  while (end_word) //while its not ending word
    {
      word_ptr = dictionary->first;
      int rand_num = get_random_number (dictionary->size);
      int i = 0;
      while (i < rand_num) //while its not the dict[i] word
        {
          word_ptr = word_ptr->next;
          ++i;
        }
      if (!word_ptr->data->is_period)
        {
          end_word = false;
        }
    }
  return word_ptr->data;
}

/**
 * Choose randomly the next word. Depend on it's occurrence frequency
 * in word_struct_ptr->WordProbability.
 * @param word_struct_ptr WordStruct to choose from
 * @return WordStruct of the chosen word
 */
WordStruct *get_next_random_word (WordStruct *word_struct_ptr)
{
  float i = (float) get_random_number (100);
  int j = 0;
  float sum = ((float) word_struct_ptr->prob_list[j].num_of_occ * 100
               / (float) word_struct_ptr->num);
  while (sum <= i)
    {
      ++j;
      sum += ((float) word_struct_ptr->prob_list[j].num_of_occ * 100
              / (float) word_struct_ptr->num);
    }
  return word_struct_ptr->prob_list[j].word_struct_ptr;
}

/**
 * Receive dictionary, generate and print to stdout random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param dictionary Dictionary to use
 * @return Amount of words in printed sentence
 */
int generate_sentence (LinkList *dictionary)
{
  char sentence[MAX_WORDS_IN_SENTENCE_GENERATION * MAX_WORD_LENGTH];
  sentence[0] = '\0';
  int i = 0;
  WordStruct *first_word = get_first_random_word (dictionary);
  while (i < MAX_WORDS_IN_SENTENCE_GENERATION)
    {
      strcat (sentence, first_word->word);
      strcat (sentence, " ");
      if (first_word->is_period == true)
        {
          ++i;
          break;
        }
      first_word = get_next_random_word (first_word);
      ++i;
    }
  sentence[strlen (sentence) - 1] = '\0';
  strcat (sentence, "\n");
  printf ("%s", sentence);

  return EXIT_SUCCESS;
}

/**
 * Gets 2 WordStructs. If second_word in first_word's prob_list,
 * update the existing probability value.
 * Otherwise, add the second word to the prob_list of the first word.
 * @param first_word
 * @param second_word
 * @return 0 if already in list, 1 otherwise.
 */
int add_word_to_probability_list (WordStruct *first_word,
                                  WordStruct *second_word)
{
  //check if in prob list
  for (int i = 0; i < first_word->diff_words; ++i)
    {
      if (!strcmp (second_word->word, first_word->prob_list[i].word_struct_ptr->word))
        {
          ++first_word->prob_list[i].num_of_occ;
          return EXIT_SUCCESS;
        }
    }
  //not in prob list
  WordProbability *temp = (WordProbability *) realloc (first_word->prob_list,
                                                       sizeof (WordProbability)
                                                       * (first_word->diff_words
                                                          + 1));
  VALIDATE(temp)
  first_word->prob_list = temp;
  first_word->prob_list[first_word->diff_words].word_struct_ptr = second_word;
  first_word->prob_list[first_word->diff_words].num_of_occ = 1;
  ++first_word->diff_words;
  return 1;
}

/**
 * Get the word, dictionary and bool to mark if it`s thw second word or the first.
 * creates new word struct, initialize word,num (if first), diff_word and mark if
 * the word end in period . also initialize probability list.
 * @param w the word
 * @param dict dictionary
 * @param second_time mark if the word is from the second word
 * @return new word struct
 */
WordStruct *creat_new_struct (char *w, LinkList *dict, bool second_time)
{
  WordStruct *word_struct = (WordStruct *) malloc (sizeof (WordStruct));
  VALIDATE(word_struct)
  word_struct->word = malloc (sizeof (char) * strlen (w) + 1);
  VALIDATE(word_struct->word)
  strcpy (word_struct->word, w);
  word_struct->num = 1;
  if (second_time)
    { word_struct->num = 0; }
  word_struct->diff_words = 0;
  word_struct->is_period = false;
  if (w[(strlen (w) - 1)] == '.')
    {
      word_struct->is_period = true;
    }
  word_struct->prob_list = (WordProbability *) malloc (sizeof (WordProbability));
  VALIDATE(word_struct->prob_list)
  word_struct->prob_list->num_of_occ = 0;
  word_struct->prob_list->word_struct_ptr = NULL;
  if (add (dict, word_struct) == EXIT_FAILURE)
    { exit (EXIT_FAILURE); }

  return word_struct;
}

/**
 *
 * @param word
 * @param dict
 * @return
 */
WordStruct *check_if_in_dict (char *word, LinkList *dict, bool second_time)
{
  Node *root = dict->first;
  while (root != NULL)
    {
      if (!strcmp (root->data->word, word))
        {
          if (!second_time)
            { ++root->data->num; }
          return root->data;
        }
      root = root->next;
    }
  return creat_new_struct (word, dict, second_time);

}
/**
 * Read word from the given file. Add every unique word to the dictionary.
 * Also, at every iteration, update the prob_list of the previous word with
 * the value of the current word.
 * @param fp File pointer
 * @param lines_to_read Number of lines to read from file.
 *                      If value is bigger than the file's lines count,
 *                      or if lines_to_read == -1 than read entire file.
 * @param dictionary Empty dictionary to fill
 */
void fill_dictionary (FILE *fp, int lines_to_read, LinkList *dictionary)
{
  char sentence[MAX_SENTENCE_LENGTH];
  int i = 0;
  while (i < lines_to_read || lines_to_read == -1)
    {
      if (fgets (sentence, MAX_SENTENCE_LENGTH, fp) != NULL)
        {
          char *token1 = strtok (sentence, " \r\n");
          char *token2 = strtok (NULL, " \r\n");
          while (token1 != NULL)
            {
              WordStruct *w1 = check_if_in_dict (token1, dictionary, false);
              if (w1->is_period == true)
                { w1->prob_list->word_struct_ptr = NULL; }
              else
                {
                  WordStruct *w2 = check_if_in_dict (token2, dictionary, true);
                  add_word_to_probability_list (w1, w2);
                }
              token1 = token2;
              token2 = strtok (NULL, " \r,\n");
            }
        }
      ++i;
      if (i == lines_to_read - 1 && i != -1)
        {
          return;
        }
    }
}

/**
 * Free the given dictionary and all of it's content from memory.
 * @param dictionary Dictionary to free
 */
void free_dictionary (LinkList *dictionary)
{
  Node *node = dictionary->first, *temp_node_ptr = NULL;
  while (node != dictionary->last)
    {
      free (node->data->prob_list);
      free (node->data->word);
      free (node->data);
      temp_node_ptr = node;
      node = node->next;
      free (temp_node_ptr);
      temp_node_ptr = NULL;
    }
  free (dictionary->last->data->word);
  free (dictionary->last->data);
  free (dictionary->last);
  free (dictionary);

}

/**
 * @param argc
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 *             3) Path to file
 *             4) Optional - Number of words to read
 */
int main (int argc, char *argv[])
{
  if (argc != 4 && argc != 5)
    {
      fprintf (stdout, MES1);
      return EXIT_FAILURE;
    }
  FILE *file;
  file = fopen (argv[3], "r");
  if (file == NULL)
    {
      fprintf (stdout, MES2);
      return EXIT_FAILURE;
    }
  int lines_to_read = -1;
  int seed;
  sscanf (argv[1], "%d", &seed);
  int num_of_sentence;
  if (argc == 5)
    {
      sscanf (argv[4], "%d", &lines_to_read);
    }
  srand (seed);
  sscanf (argv[2], "%d", &num_of_sentence);
  //create dictionary and initialize
  LinkList *dict = (LinkList *) malloc (sizeof (LinkList));
  VALIDATE(dict)
  dict->size = 0;
  dict->first = NULL;
  dict->last = NULL;
  //fill dictionary
  fill_dictionary (file, lines_to_read, dict);
  fclose (file);
  //create sentences
  for (int j = 1; j <= num_of_sentence; ++j)
    {
      printf ("Tweet %d: ", j);
      generate_sentence (dict);
    }
  free_dictionary (dict);
  return EXIT_SUCCESS;
}

