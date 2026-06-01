@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%
echo Proje derleniyor... Lutfen bekleyin.
gcc main.c graph_models.c graph_adj.c hash_table.c trie.c queue.c algorithms.c queries.c data_generator.c ui_render.c ui_integration.c -o program_adi.exe -lraylib -lopengl32 -lgdi32 -lwinmm
echo Derleme islemi bitti. Eger yukarida 'error' yazmiyorsa arayuz aciliyor...
program_adi.exe
pause