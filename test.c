#include <mxml.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t
#define f32 float
#define f64 double

static const char *xml_whitespace_cb(mxml_node_t *node, int where) {
    if (where == MXML_WS_AFTER_OPEN || where == MXML_WS_AFTER_CLOSE)
        return ("\n");

    return (NULL);
}

int main() {
#ifdef __PS4__
    Notify("%s", __TIME__);
    char *buffer;
    char *buffer2;
    u64 size = 0;
    u64 size2 = 0;
    char *input_file = "/data/GoldHEN/test.xml";
    s32 res = Read_File(input_file, &buffer, &size, 0);

    if (res) {
        Notify("file %s not found\n error: 0x%08x", input_file, res);
        return 0;
    }
#endif

#ifdef __PC__

    // puts("hello");
    char *input_file = "text.xml";
    char *buffer = 0;
    u64 length = 0;
    FILE *f = fopen(input_file, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }

#endif
    if (buffer)
    {
        mxml_node_t *node, *tree, *Patchlist_node, *Line_node, *GameData, *IDNode = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree) {
            printf("XML: could not parse XML:\n%s\n", buffer);
            free(buffer);
            return 1;
        }

        GameData = mxmlFindElement(tree, tree, "Patch", NULL, NULL, MXML_DESCEND);
        printf("Game: %s\n", mxmlElementGetAttr(GameData, "Game"));
        u32 ID_idx = 1;
        for (IDNode = mxmlFindElement(tree, tree, "ID", NULL, NULL, MXML_DESCEND); IDNode != NULL;
             IDNode = mxmlFindElement(IDNode, tree, "ID", NULL, NULL, MXML_DESCEND))
        {
            // const char* IDText = mxmlGetText(IDNode, NULL);
            printf("Title IDs(%u): %s\n", ID_idx, mxmlGetText(IDNode, NULL));
            ID_idx++;
        }

        u32 idx = 1;
        for (node = mxmlFindElement(tree, tree, "Metadata", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "Metadata", NULL, NULL, MXML_DESCEND))
        {
            mxml_node_t *NameNode = mxmlFindElement(node, tree, "Name", NULL, NULL, MXML_DESCEND);
            mxml_node_t *AuthorNode = mxmlFindElement(node, tree, "Author", NULL, NULL, MXML_DESCEND);
            mxml_node_t *NoteNode = mxmlFindElement(node, tree, "Note", NULL, NULL, MXML_DESCEND);
            const char *NameData = mxmlElementGetAttr(NameNode, "Text");
            const char *AuthorData = mxmlElementGetAttr(AuthorNode, "Text");
            const char *NoteData = mxmlElementGetAttr(NoteNode, "Text");
            /*
            const char *NameData = mxmlElementGetAttr(NameNode, "Text");
            if (strcmp(NameData, "InfShield") == 0 || strcmp(NameData, "InfCrystal") == 0)
            {
                printf("%s found\n", NameData);
                continue;
            }
            */
            if (NameData == NULL)
                NameData = "(blank)";
            if (AuthorData == NULL)
                AuthorData = "(blank)";
            if (NoteData == NULL)
                NoteData = "(blank)";
            printf("Author: \"%s\" ", AuthorData);
            printf("Name: \"%s\" ", NameData);
            printf("Note: \"%s\"\n", NoteData);
            Patchlist_node = mxmlFindElement(node, node, "PatchList", NULL, NULL, MXML_DESCEND);
            for (Line_node = mxmlFindElement(node, node, "Line", NULL, NULL, MXML_DESCEND); Line_node != NULL;
                 Line_node = mxmlFindElement(Line_node, Patchlist_node, "Line", NULL, NULL, MXML_DESCEND))
            {
                printf("Offset: \"%s\" ", mxmlElementGetAttr(Line_node, "Offset"));
                printf("ValueOn: \"%s\" ", mxmlElementGetAttr(Line_node, "ValueOn"));
                printf("ValueOff: \"%s\"\n", mxmlElementGetAttr(Line_node, "ValueOff"));
                //printf("patch line: %u\n", idx);
                idx++;
            }
        }
        mxmlDelete(Line_node);
        mxmlDelete(Patchlist_node);
        mxmlDelete(IDNode);
        mxmlDelete(GameData);
        mxmlDelete(node);
        mxmlDelete(tree);
        free(buffer);
        printf("Build Time: %s\n", __TIME__);
        return 0;
    }
    printf("File %s empty\n", input_file);
    return 1;
}
