# Directree 🌲
## Directory Tree Generator
This is was meant to be a one hour project to try out the `dirent.h` library. But the files were given in a random manner rather than in an ordered sequence (alphabetically, according to size etc).


So I spent some extra hours implementing a sorting system. I stored each file in a linked list and sorted it alphabetically while adding it. Seems pretty easy but I was faced with a problem when trying to maniplate the linked list from a function. It turns out that a pointer is passed by value (as is everything in C), so you can't change the address it is pointing to unless you use a pointer to the pointer (double pointer) 🥴.

Great learning experience for me! 🥳

## Things I learnt
1. Getting files and directories using `<dirent.h>` 
2. Double pointers.
3. Inserting a node at any point in a linked list.
4. Getting file information using `<sys/stat.h>`
5. Printing coloured output.

## Try it!
- Clone the repo
- Type `make`
- An executable should be created
- Run it by typing, `./directree`
    - You can specify a folder: `./directree 'folder'`
