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
    char *input_file = "text2.xml";
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
        mxml_node_t *node, *tree, *Patchlist_node, *Line_node, *GameData, *IDNode = NULL;
        tree = mxmlLoadString(NULL, buffer, MXML_NO_CALLBACK);

        if (!tree) {
            printf("XML: could not parse XML:\n%s\n", buffer);
            free(buffer);
            return 1;
        }

        GameData = mxmlFindElement(tree, tree, "Patch", NULL, NULL, MXML_DESCEND);
        const char *GameName = mxmlElementGetAttr(GameData, "Game");
        printf("Game: %s\n", GameName);
        u32 ID_idx = 1;

        for (IDNode = mxmlFindElement(tree, tree, "ID", NULL, NULL, MXML_DESCEND); IDNode != NULL;
             IDNode = mxmlFindElement(IDNode, tree, "ID", NULL, NULL, MXML_DESCEND)) {
            printf("Title IDs(%u): %s\n", ID_idx, mxmlGetText(IDNode, NULL));
            ID_idx++;
        }

        for (node = mxmlFindElement(tree, tree, "Metadata", NULL, NULL, MXML_DESCEND); node != NULL;
             node = mxmlFindElement(node, tree, "Metadata", NULL, NULL, MXML_DESCEND)) {
            const char *NameData = mxmlElementGetAttr(node, "Name");
            const char *AuthorData = mxmlElementGetAttr(node, "Author");
            const char *NoteData = mxmlElementGetAttr(node, "Note");
            const char *AppVerData = mxmlElementGetAttr(node, "AppVer");
            const char *AppElfData = mxmlElementGetAttr(node, "AppElf");
            if (NameData == NULL)
                NameData = "(blank)";
            if (AuthorData == NULL)
                AuthorData = "(blank)";
            if (NoteData == NULL)
                NoteData = "(blank)";
            if (AppVerData == NULL)
                AppVerData = "(blank)";
            if (AppElfData == NULL)
                AppElfData = "(blank)";
            printf("Name: \"%s\" ", NameData);
            printf("Author: \"%s\" ", AuthorData);
            printf("AppVer: \"%s\"", AppVerData);
            printf("AppElf: \"%s\"", AppElfData);
            printf("Note: \"%s\"\n", NoteData);
/*
            u64 hashout = patch_hash_calc(NameData, GameName, AppVerData, input_file, AppElfData);
            char settings_path[64];
            snprintf(settings_path, sizeof(settings_path), "/data/GoldHEN/patches/settings/0x%016lx.txt", hashout);
            final_printf("Settings path: %s\n", settings_path);
            s32 res = Read_File(settings_path, &buffer2, &size2, 0);
            if (res == 0x80020002) {
                debug_printf("file %s not found, initializing false. ret: 0x%08x\n", settings_path, res);
                unsigned char false_data[2] = {0x30, 0xa};
                Write_File(settings_path, false_data, sizeof(false_data));
            } else if (buffer2[0] == '1' && !strcmp(game_ver, AppVerData) && !strcmp(game_elf, AppElfData)) {
*/
                Patchlist_node = mxmlFindElement(node, node, "PatchList", NULL, NULL, MXML_DESCEND);
                for (Line_node = mxmlFindElement(node, node, "Line", NULL, NULL, MXML_DESCEND); Line_node != NULL;
                     Line_node = mxmlFindElement(Line_node, Patchlist_node, "Line", NULL, NULL, MXML_DESCEND)) {
                    u64 addr_real = 0;
                    const char *gameType = mxmlElementGetAttr(Line_node, "Type");
                    const char *gameAddr = mxmlElementGetAttr(Line_node, "Address");
                    const char *gameValue = mxmlElementGetAttr(Line_node, "Value");
                    printf("Type: \"%s\" ", gameType);
                    if (gameAddr) {
                        addr_real = strtoull(gameAddr, NULL, 16);
                        printf("Address: 0x%lx ", addr_real);
                    }
                    printf("Value: \"%s\"\n", gameValue);
                    // printf("patch line: %lu\n", patch_lines);
/*
                    if (gameType && addr_real) // type and address must be present
                    {
                        patch_data1(gameType, addr_real, gameValue);
                        patch_lines++;
                    }
                    patch_items++;
*/
                //}
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
