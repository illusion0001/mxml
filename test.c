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
    if (buffer) {
        mxml_node_t *node, *tree, *Cheatline_node, *Offset_node = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree) {
            printf("XML: could not parse XML: %s\n", buffer);
            free(buffer);
            return 1;
        }
        u32 idx = 1;
        for (node = mxmlFindElement(tree, tree, "PatchData", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "PatchData", NULL, NULL, MXML_DESCEND))
        {
            Cheatline_node = mxmlFindElement(node, node, "PatchList", NULL, NULL, MXML_DESCEND);
            for (Offset_node = mxmlFindElement(node, node, "Line", NULL, NULL, MXML_DESCEND); Offset_node != NULL;
                 Offset_node = mxmlFindElement(Offset_node, Cheatline_node, "Line", NULL, NULL, MXML_DESCEND))
            {
                printf("%s\n", mxmlElementGetAttr(Offset_node, "Offset"));
                printf("%s\n", mxmlElementGetAttr(Offset_node, "ValueOn"));
                printf("%s\n", mxmlElementGetAttr(Offset_node, "ValueOff"));
                printf("patch line: %u\n", idx);
                idx++;
            }
        }
        mxmlDelete(Offset_node);
        mxmlDelete(Cheatline_node);
        mxmlDelete(node);
        mxmlDelete(tree);
        free(buffer);
        printf("Build Time: %s\n", __TIME__);
        return 0;
    }
    printf("File %s empty\n", input_file);
    return 1;
}
