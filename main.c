#include <stdio.h> // printf, sprintf
#include <sys/stat.h> // stat, S_ISDIR, S_IXUSR
#include <stdlib.h> // calloc, free, exit
#include <string.h> // strlen, strncpy
#include <strings.h> // strcasecmp
#include <dirent.h> // opendir, readdir, closedir

enum FileType { EXEC = 2, DIREC = 4, REG = 8 };

typedef struct Listing
{
    char name[512 + 1];
    int fileType; // regular, dir, executable
    struct Listing *next; // Next listing (folder or file)
    struct Listing *child; // For folders
} Listing;

int files = 0, directories = 0;
Listing *root = NULL;

int GetFileType(const char *path);
void GetDirs(const char *dirPath, Listing **start);
void PrintColour(int fileType);
void PrintListings(Listing *start, int depth);
void FreeListings(Listing *start);

int main(int argc, char const *argv[])
{
    char dirPath[512 + 1] = ".";
    // If argument given, change dirPath
    if (argc > 1) strncpy(dirPath, argv[1], 512);
    
    // Check if dirPath is a directory
    if (GetFileType(dirPath) == DIREC)
    {
        // Get directories and files and store in a list
        GetDirs(dirPath, &root);

        // Print the first path in colour
        PrintColour(DIREC);
        printf("%s\n", dirPath);
        PrintColour(0);

        // Print each listing
        PrintListings(root, 1);
        // Print total directories and files
        printf("\n%i %s, %i %s\n", directories, (directories == 1) ? "directory" : "directories", files, (files == 1) ? "file" : "files");

        FreeListings(root);
    }

    // Not a directory (a file)
    else
    {
        printf("'%s' is not a directory\n", dirPath);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int GetFileType(const char *path)
{
    struct stat pathStat;
    // Get file attributes (if nothing returned, path does not exit)
    if (stat(path, &pathStat) != 0)
    {
        printf("Path does not exist: %s\n", path);
        exit(EXIT_FAILURE);
    }
    
    // Folder
    if (S_ISDIR(pathStat.st_mode)) return DIREC;
    // Executable
    else if (pathStat.st_mode & S_IXUSR) return EXEC;
    // Regular file
    return REG;
}

void AddListing(Listing *new, Listing **start)
{
    // First listing
    if (*start == NULL) *start = new;
    // Insert in linked list based on name
    else
    {
        Listing **traverse = start, *previous = NULL;
        while ((*traverse) != NULL)
        {
            // If new guy's name appears before current guy's name
            // Insert new guy at current position
            if (strcasecmp(new->name, (*traverse)->name) < 0)
            {
                // New guy's next should point to current
                new->next = *traverse;
                // If this is the first position, set beginning (start) to new guy
                if (previous == NULL) (*traverse) = new;
                // Else set the previous guy to point to new guy
                else previous->next = new;
                return; // Exit function
            }
            // Save current
            previous = *traverse;
            // Stop from getting to NULL
            if ((*traverse)->next == NULL)  break;
            // Move to next listing (since it's a double pointer, get address)
            traverse = &((*traverse)->next);
        }
        // If new guy's name is greater lexicographically than all others, insert it at the end
        (*traverse)->next = new;
    }
}

void GetDirs(const char *path, Listing **start)
{
    struct dirent *file;
    DIR *dir = opendir(path);

    if (dir != NULL)
    {
        // While a file in directory
        while ((file = readdir(dir)))
        {
            // Skip dot files
            if (file->d_name[0] == '.') continue;

            char currentPath[512 + strlen(file->d_name) + 1];
            // Set current path (path/file name)
            sprintf(currentPath, "%s/%s", path, file->d_name);

            // Allocate memory for a new listing
            Listing *newListing = (Listing *) calloc(1, sizeof(Listing));
            // Set file type
            newListing->fileType = GetFileType(currentPath);
            // Set newListing name
            strncpy(newListing->name, file->d_name, 512);
            // If a folder
            if (file->d_type == DIREC)
            {
                directories++;
                // Set child of current listing
                GetDirs(currentPath, &newListing->child);
            }
            // Else a file
            else files++;
            // Add newListing to linked list
            AddListing(newListing, start);
        }
        closedir(dir);
    }

    else
    {
        printf("Unable to open directory\n");
        exit(EXIT_FAILURE);
    }
}

// Change terminal colour based on file type
void PrintColour(int fileType)
{
    switch (fileType)
    {
        // Cyan colour
        case DIREC:
            printf("\033[0;36m");
            break;
        
        // Purple colour
        case EXEC:
            printf("\033[0;35m");
            break;
        
        // Reset colour
        default:
            printf("\033[0m");
            break;
    }
}

void PrintListings(Listing *start, int depth)
{
    // Moving variable
    Listing *traverse = start;
    while (traverse != NULL)
    {
        // Print vertical bars to indicate depth, and at last bar print arrow to lead to file
        for (int i = 1; i <= depth; i++) printf("▏%s", (i == depth) ? "⟶ " : "  ");
        // Colour based on file type
        PrintColour(traverse->fileType);
        // Print file name
        printf("%s\n", traverse->name);
        PrintColour(0); // reset
        // If a folder, print children with increased depth
        if (traverse->child != NULL) PrintListings(traverse->child, depth + 1);
        // Go to next listing
        traverse = traverse->next;
    }
}

void FreeListings(Listing *start)
{
    // Temp variables for storing current and next listings
    Listing *current = start, *next = start;
    while (current != NULL)
    {
        // Set next
        next = current->next;
        // If current is a folder, free children
        if (current->child != NULL) FreeListings(current->child);
        free(current);
        // Go to next listing
        current = next;
    }
}
