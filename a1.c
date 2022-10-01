
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define MAX_LINE_SIZE 10000

void listRec(char *path, int sizeSmaller, char *nameStart, int okName)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char fullPath[512];
	struct stat statbuf;

	dir = opendir(path);
	if (dir == NULL)
	{
		printf("ERROR\n");
		printf("invalid directory path\n");
		return;
	}
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			// if(nameStart == NULL || strncmp(nameStart, entry->d_name, lengthName)) {
			snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
			if (lstat(fullPath, &statbuf) == 0)
			{
				// printf("%s\n", fullPath);

				char subString[25];
				if (nameStart != NULL)
				{
					// subString = (char *)malloc((strlen(nameStart) + 1) * sizeof(char));
					if (strlen(entry->d_name) >= strlen(nameStart))
					{
						snprintf(subString, strlen(nameStart) + 1, "%s", entry->d_name);
					}
					// printf("Sirul de comparat: %s \n", subString);
				}

				if (okName == 0 || strcmp(subString, nameStart) == 0)
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						printf("%s\n", fullPath);
						listRec(fullPath, sizeSmaller, nameStart, okName);
					}
					else
					{
						if (sizeSmaller != -1)
						{
							if (statbuf.st_size < sizeSmaller)
							{
								printf("%s\n", fullPath);
							}
						}
						else
						{
							printf("%s\n", fullPath);
						}
					}
				}
				else if (S_ISDIR(statbuf.st_mode))
				{
					listRec(fullPath, sizeSmaller, nameStart, okName);
				}
				// free(subString);
			}
			//}
		}
	}

	closedir(dir);
}

int listDir(char *path, int sizeSmaller, char *nameStart, int okName)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char filePath[512];
	struct stat statbuf;

	// int lengthName = strlen(nameStart);

	dir = opendir(path);
	if (dir == NULL)
	{
		printf("ERROR\n");
		printf("invalid directory path\n");
		return -1;
	}
	while ((entry = readdir(dir)) != NULL)
	{
		snprintf(filePath, 512, "%s/%s", path, entry->d_name);

		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			// if(nameStart == NULL || strncmp(nameStart, entry->d_name, lengthName)) {
			if (lstat(filePath, &statbuf) == 0)
			{
				// printf("%s/%s\n", path, entry->d_name);

				char subString[25];
				if (nameStart != NULL)
				{
					if (strlen(entry->d_name) >= strlen(nameStart))
					{
						snprintf(subString, strlen(nameStart) + 1, "%s", entry->d_name);
					}
					// printf("Sirul de comparat: %s \n", subString);
				}

				if (okName == 0 || strcmp(subString, nameStart) == 0)
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						printf("%s/%s\n", path, entry->d_name);
					}
					else
					{
						if (sizeSmaller != -1)
						{
							if (statbuf.st_size < sizeSmaller)
							{
								printf("%s/%s\n", path, entry->d_name);
							}
						}
						else
						{
							printf("%s/%s\n", path, entry->d_name);
						}
					}
				}

				// free(subString);
			}
			//}
		}
	}

	closedir(dir);

	return 0;
}

int parseFile(int fd)
{
	char magic[3] = {0};
	char header_size[3];
	char version;
	char nr_sections;
	char sect_name[10] = {0};
	char sect_type;
	char sect_offset[5];
	int sect_size = 0;

	int citireMagic = read(fd, magic, 2);
	if (strcmp(magic, "vK") != 0)
	{
		printf("ERROR\n");
		printf("wrong magic\n");
		close(fd);
		return -12;
	}
	else if (citireMagic < 2)
	{
		printf("SUCCESS\n");
		close(fd);
		return -13;
	}
	magic[2] = '\0';

	int citireHeader = read(fd, header_size, 2);
	if (citireHeader < 2)
	{
		printf("SUCCESS\n");
		close(fd);
		return -14;
	}
	header_size[2] = '\0';

	int citireVersion = read(fd, &version, 2);
	if (citireVersion == 2)
	{
		if (version < 87 || version > 118)
		{
			printf("ERROR\n");
			printf("wrong version\n");
			close(fd);
			return -15;
		}
		else if (citireVersion < 2)
		{
			printf("SUCCESS\n");
		}
	}

	int citireNrSections = read(fd, &nr_sections, 1);

	if (citireNrSections < 1)
	{
		printf("SUCCESS\n");
		printf("version=%c\n", version);
	}
	else if (citireNrSections == 1)
	{
		if (nr_sections < 2 || nr_sections > 19)
		{
			printf("ERROR\n");
			printf("wrong sect_nr\n");
			close(fd);
			return -16;
		}
	}

	for (char i = 0; i < nr_sections; i++)
	{
		read(fd, sect_name, 9);
		sect_name[9] = '\0';
		read(fd, &sect_type, 1);
		if (sect_type != 30 && sect_type != 96 && sect_type != 68 && sect_type != 38)
		{
			printf("ERROR\n");
			printf("wrong sect_types\n");
			close(fd);
			return -17;
		}
		read(fd, sect_offset, 4);
		sect_offset[4] = '\0';
		read(fd, &sect_size, 4);
	}
	int aux = (int)version;
	int aux2 = (int)nr_sections;
	printf("SUCCESS\n");
	printf("version=%d \n", aux);
	printf("nr_sections=%d\n", aux2);

	lseek(fd, 7, SEEK_SET);

	for (char i = 0; i < nr_sections; i++)
	{
		// ok = 1;
		printf("section%d: ", i + 1);
		read(fd, sect_name, 9);
		sect_name[9] = '\0';
		read(fd, &sect_type, 1);
		int val = (int)sect_type;
		read(fd, sect_offset, 4);
		sect_offset[4] = '\0';
		read(fd, &sect_size, 4);
		printf("%s %d %d\n", sect_name, val, sect_size);
	}

	close(fd);

	return 1;
}

int extractFile(int fd, int section, int line)
{


	char nr_sections = 0;

	lseek(fd, 6, SEEK_SET);

	read(fd, &nr_sections, 1);

	lseek(fd, 18 * (section - 1) + 10, SEEK_CUR);

	int sect_offset = 0;
	int sect_size = 0;

	read(fd, &sect_offset, 4);
	read(fd, &sect_size, 4);

	int indexLine = 1;
	int j = 0;

	char lineExtract[sect_size];

	lseek(fd, sect_offset, SEEK_SET);

	char *buffer = (char *) calloc (sect_size + 1, sizeof(char));

	read(fd, buffer, sect_size);

	int bufferLength = strlen(buffer);

	for(int i = 0; i < bufferLength; i++) {
		if(indexLine == line) {
			lineExtract[j++] = buffer[i];
		}
		if(buffer[i - 1] == 0x0D && buffer[i] == 0x0A) {
			indexLine++;
		}
		if(buffer[i] == 0) {
			indexLine++;
			break;
		}
	}

	lineExtract[j] = 0;

	printf("SUCCESS\n");
	printf("%s\n", lineExtract);

	close(fd);

	return 1;
}

void findallRec(const char *path)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char fullPath[512];
	struct stat statbuf;

	dir = opendir(path);
	if (dir == NULL)
	{
		printf("ERROR\n");
		printf("invalid directory path\n");
		return;
	}
	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
			if (lstat(fullPath, &statbuf) == 0)
			{
				// printf("%s\n", fullPath);
				if (S_ISDIR(statbuf.st_mode))
				{
					findallRec(fullPath);
				}
				else
				{
					int fd = -1;
					fd = open(fullPath, O_RDONLY);

					if (fd == -1)
					{
						perror("Nu am reusit sa deschid fisierul primit ca path");
						return;
					}

					char magic[3] = {0};
					char header_size[3];
					char version;
					char nr_sections;
					char sect_name[10] = {0};
					char sect_type;
					char sect_offset[5];
					int sect_size = 0;

					int ok = 0;

					read(fd, magic, 2);
					if (strcmp(magic, "vK") != 0)
					{
						ok++;
					}
					magic[2] = '\0';

					read(fd, header_size, 2);
					header_size[2] = '\0';

					int citireVersion = read(fd, &version, 2);
					if (citireVersion == 2)
					{
						if (version < 87 || version > 118)
						{
							ok++;
						}
					}

					int citireNrSections = read(fd, &nr_sections, 1);

					if (citireNrSections == 1)
					{
						if (nr_sections < 2 || nr_sections > 19)
						{
							ok++;
						}
					}

					int sizeBigger = 0;

					for (char i = 0; i < nr_sections; i++)
					{
						read(fd, sect_name, 9);
						sect_name[9] = '\0';
						read(fd, &sect_type, 1);
						if (sect_type != 30 && sect_type != 96 && sect_type != 68 && sect_type != 38)
						{
							ok++;
						}
						read(fd, sect_offset, 4);
						sect_offset[4] = '\0';
						read(fd, &sect_size, 4);
						if (sect_size > 1346)
						{
							sizeBigger++;
						}
					}

					if (ok == 0 && sizeBigger == 0)
					{
						// if(sizeBigger == 0){
						printf("%s\n", fullPath);
					}

					close(fd);
				}
			}
		}
	}

	closedir(dir);
}

int main(int argc, char **argv)
{
	int pozArgumente = 2;
	int pozArgumenteNew = 2;
	int parcurgereRecursiva = 0;
	int sizeSmaller = -1;

	char *path = NULL;
	char *pathFile = NULL;
	char *nameStartsWith = NULL;
	int section = 0;
	int line = 0;

	struct stat statbuf;

	if (argc >= 2)
	{

		int okPath = 0; // pentru a ne asigura ca primim la linia de comanda path-ul
		int okName = 0;

		if (strcmp(argv[1], "variant") == 0)
		{
			printf("86676\n");
		}
		else if (strcmp(argv[1], "list") == 0)
		{
			while (pozArgumente < argc)
			{
				int lungimeSize = strlen("size_smaller=");
				int lungimePath = strlen("path=");
				int lungimeName = strlen("name_starts_with=");

				if (strcmp(argv[pozArgumente], "recursive") == 0)
				{
					parcurgereRecursiva = 1;
					// printf("Parcurgere recursiva: %d\n", parcurgereRecursiva);
				}
				else if (strncmp(argv[pozArgumente], "size_smaller=", lungimeSize) == 0)
				{
					sizeSmaller = atoi(argv[pozArgumente] + lungimeSize);
					// printf("Size_smaller = %d \n", sizeSmaller);
				}
				else if (strncmp(argv[pozArgumente], "name_starts_with=", lungimeName) == 0)
				{
					nameStartsWith = argv[pozArgumente] + lungimeName;
					// printf("name_Starts_With = %s \n", nameStartsWith);
					okName = 1;
				}
				else if (strncmp(argv[pozArgumente], "path=", lungimePath) == 0)
				{
					path = argv[pozArgumente] + lungimePath;

					if (path != NULL)
					{
						okPath = 1;
					}

					// printf("path = %s \n", path);
					// printf("am reusit sa obtinem path-ul: %d \n", okPath);
				}

				pozArgumente++;
			}

			if (okPath == 0 || stat(path, &statbuf) < 0 || !S_ISDIR(statbuf.st_mode))
			{
				printf("ERROR\n");
				printf("invalid directory path\n");
				// perror("Am intalnit o eroare la primirea path-ului");
			}
			else
			{
				printf("SUCCESS\n");

				if (parcurgereRecursiva == 1)
				{
					listRec(path, sizeSmaller, nameStartsWith, okName);
				}
				else
				{
					listDir(path, sizeSmaller, nameStartsWith, okName);
				}
			}
		}
		else if (strcmp(argv[1], "parse") == 0)
		{
			if (strncmp(argv[2], "path=", 5) == 0)
			{
				pathFile = argv[2] + 5;
				// printf("path = %s \n", pathFile);
			}

			int fd = -1;
			fd = open(pathFile, O_RDONLY);

			if (fd == -1)
			{
				printf("ERROR\n");
				printf("invalid directory path\n");
				return -10;
			}

			parseFile(fd);
		}
		else if (strcmp(argv[1], "extract") == 0)
		{
			int countA = 0;
			
			while (pozArgumenteNew < argc)
			{
				if (strncmp(argv[pozArgumenteNew], "path=", 5) == 0)
				{
					pathFile = argv[pozArgumenteNew] + 5;
					// printf("path = %s \n", pathFile);
					countA++;
				}
				else if (strncmp(argv[pozArgumenteNew], "section=", 8) == 0)
				{
					section = atoi(argv[pozArgumenteNew] + 8);
					// printf("section = %d \n", section);
					countA++;
				}
				else if (strncmp(argv[pozArgumenteNew], "line=", 5) == 0)
				{
					line = atoi(argv[pozArgumenteNew] + 5);
					// printf("line = %d \n", line);
					countA++;
				}

				pozArgumenteNew++;
			}
			if (countA == 3)
			{
				int fd = -1;
				fd = open(pathFile, O_RDONLY);

				if (fd == -1)
				{
					// perror("Nu am reusit sa deschid fisierul");
					printf("ERROR\n");
					printf("invalid file\n");
					return -20;
				}

				extractFile(fd, section, line);
			}
			else
			{
				printf("ERROR\n");
				printf("invalid file|section|line\n");
				return -7;
			}
		}
		else if (strcmp(argv[1], "findall") == 0)
		{
			if (strncmp(argv[2], "path=", 5) == 0)
			{
				pathFile = argv[2] + 5;
				// printf("path = %s \n", pathFile);
				printf("SUCCESS\n");
				findallRec(pathFile);
			}
			else
			{
				printf("ERROR\n");
				printf("invalid directory path\n");
			}
		}
	}

	return 0;
}
