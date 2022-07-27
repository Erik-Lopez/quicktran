#ifndef _TRANSLATE_H
#define _TRANSLATE_H

void translate(struct Box **boxes)
{
	char *src_lang = boxes[0]->content;
	char *src_text = boxes[1]->content;
	char *dest_lang = boxes[2]->content;
	int *dest_text = (int*)boxes[3]->content;

	box_clear(boxes[3]);

	char *options = "-brief -no-ansi -no-warn";

#define BUFSIZE (strlen(options) + LANG_CAP*2 + TEXT_CAP*2 + 1)
	char command[BUFSIZE];
	snprintf(command, BUFSIZE, "trans -from \"%s\" -to \"%s\" \"%s\" %s", 
		src_lang, dest_lang, src_text, options);

	FILE *pipe = popen(command, "r");

	int i;
	int ch;
	for (i = 0; (ch = fgetc(pipe)) != -1; i++)
		dest_text[i] = ch;
	dest_text[i - 1] = 0; // remove '\n' left by trans

	pclose(pipe);

	box_addchstr(boxes[3], dest_text);
	box_refreshwins(boxes);
}

#endif
