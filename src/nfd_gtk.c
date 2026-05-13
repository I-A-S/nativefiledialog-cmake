/*
  Native File Dialog

  http://www.frogtoss.com/labs
*/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>
#include "nfd.h"
#include "nfd_common.h"


const char INIT_FAIL_MSG[] = "gtk_init_check failed to initialize GTK+";

struct DialogData {
    nfdresult_t result;
    nfdchar_t **outPath;
    nfdpathset_t *outPaths;
    GMainLoop *loop;
};

static void OnOpenResponse(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    struct DialogData *data = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_open_finish(dialog, res, &error);

    if (file) {
        char *filename = g_file_get_path(file);
        if (filename) {
            size_t len = strlen(filename);
            *data->outPath = NFDi_Malloc(len + 1);
            if (*data->outPath) {
                memcpy(*data->outPath, filename, len + 1);
                data->result = NFD_OKAY;
            } else {
                data->result = NFD_ERROR;
            }
            g_free(filename);
        }
        g_object_unref(file);
    } else {
        if (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
            data->result = NFD_CANCEL;
        } else {
            NFDi_SetError(error->message);
            data->result = NFD_ERROR;
        }
        g_error_free(error);
    }
    g_main_loop_quit(data->loop);
}

static void OnSaveResponse(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    struct DialogData *data = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(dialog, res, &error);

    if (file) {
        char *filename = g_file_get_path(file);
        if (filename) {
            size_t len = strlen(filename);
            *data->outPath = NFDi_Malloc(len + 1);
            if (*data->outPath) {
                memcpy(*data->outPath, filename, len + 1);
                data->result = NFD_OKAY;
            } else {
                data->result = NFD_ERROR;
            }
            g_free(filename);
        }
        g_object_unref(file);
    } else {
        if (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
            data->result = NFD_CANCEL;
        } else {
            NFDi_SetError(error->message);
            data->result = NFD_ERROR;
        }
        g_error_free(error);
    }
    g_main_loop_quit(data->loop);
}

static void OnFolderResponse(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    struct DialogData *data = user_data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_select_folder_finish(dialog, res, &error);

    if (file) {
        char *filename = g_file_get_path(file);
        if (filename) {
            size_t len = strlen(filename);
            *data->outPath = NFDi_Malloc(len + 1);
            if (*data->outPath) {
                memcpy(*data->outPath, filename, len + 1);
                data->result = NFD_OKAY;
            } else {
                data->result = NFD_ERROR;
            }
            g_free(filename);
        }
        g_object_unref(file);
    } else {
        if (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
            data->result = NFD_CANCEL;
        } else {
            NFDi_SetError(error->message);
            data->result = NFD_ERROR;
        }
        g_error_free(error);
    }
    g_main_loop_quit(data->loop);
}

static nfdresult_t AllocPathSet( GSList *fileList, nfdpathset_t *pathSet )
{
    size_t bufSize = 0;
    GSList *node;
    nfdchar_t *p_buf;
    size_t count = 0;
    
    assert(fileList);
    assert(pathSet);

    pathSet->count = (size_t)g_slist_length( fileList );
    assert( pathSet->count > 0 );

    pathSet->indices = NFDi_Malloc( sizeof(size_t)*pathSet->count );
    if ( !pathSet->indices )
    {
        return NFD_ERROR;
    }

    /* count the total space needed for buf */
    for ( node = fileList; node; node = node->next )
    {
        assert(node->data);
        bufSize += strlen( (const gchar*)node->data ) + 1;
    }

    pathSet->buf = NFDi_Malloc( sizeof(nfdchar_t) * bufSize );

    /* fill buf */
    p_buf = pathSet->buf;
    for ( node = fileList; node; node = node->next )
    {
        nfdchar_t *path = (nfdchar_t*)(node->data);
        size_t byteLen = strlen(path)+1;
        ptrdiff_t index;
        
        memcpy( p_buf, path, byteLen );
        g_free(node->data);

        index = p_buf - pathSet->buf;
        assert( index >= 0 );
        pathSet->indices[count] = (size_t)index;

        p_buf += byteLen;
        ++count;
    }

    g_slist_free( fileList );
    
    return NFD_OKAY;
}

static void OnOpenMultipleResponse(GObject *source_object, GAsyncResult *res, gpointer user_data) {
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    struct DialogData *data = user_data;
    GError *error = NULL;
    GListModel *files = gtk_file_dialog_open_multiple_finish(dialog, res, &error);

    if (files) {
        guint count = g_list_model_get_n_items(files);
        GSList *fileList = NULL;
        for (guint i = 0; i < count; i++) {
            GFile *file = g_list_model_get_item(files, i);
            if (file) {
                char *filename = g_file_get_path(file);
                if (filename) {
                    fileList = g_slist_append(fileList, filename);
                }
                g_object_unref(file);
            }
        }
        if (fileList) {
            if (AllocPathSet(fileList, data->outPaths) == NFD_OKAY) {
                data->result = NFD_OKAY;
            } else {
                data->result = NFD_ERROR;
            }
        }
        g_object_unref(files);
    } else {
        if (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED)) {
            data->result = NFD_CANCEL;
        } else {
            NFDi_SetError(error->message);
            data->result = NFD_ERROR;
        }
        g_error_free(error);
    }
    g_main_loop_quit(data->loop);
}

static void AddTypeToFilterName( const char *typebuf, char *filterName, size_t bufsize )
{
    const char SEP[] = ", ";

    size_t len = strlen(filterName);
    if ( len != 0 )
    {
        strncat( filterName, SEP, bufsize - len - 1 );
        len += strlen(SEP);
    }
    
    strncat( filterName, typebuf, bufsize - len - 1 );
}

static void AddFiltersToDialog( GtkFileDialog *dialog, const char *filterList )
{
    GtkFileFilter *filter;
    GListStore *filters;
    char typebuf[NFD_MAX_STRLEN] = {0};
    const char *p_filterList = filterList;
    char *p_typebuf = typebuf;
    char filterName[NFD_MAX_STRLEN] = {0};
    
    if ( !filterList || strlen(filterList) == 0 )
        return;

    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);

    filter = gtk_file_filter_new();
    while ( 1 )
    {
        
        if ( NFDi_IsFilterSegmentChar(*p_filterList) )
        {
            char typebufWildcard[NFD_MAX_STRLEN + 2];
            /* add another type to the filter */
            assert( strlen(typebuf) > 0 );
            assert( strlen(typebuf) < NFD_MAX_STRLEN-1 );
            
            snprintf( typebufWildcard, NFD_MAX_STRLEN + 2, "*.%s", typebuf );
            AddTypeToFilterName( typebuf, filterName, NFD_MAX_STRLEN );
            
            gtk_file_filter_add_pattern( filter, typebufWildcard );
            
            p_typebuf = typebuf;
            memset( typebuf, 0, sizeof(char) * NFD_MAX_STRLEN );
        }
        
        if ( *p_filterList == ';' || *p_filterList == '\0' )
        {
            /* end of filter -- add it to the dialog */
            
            gtk_file_filter_set_name( filter, filterName );
            g_list_store_append( filters, filter );
            g_object_unref( filter );

            filterName[0] = '\0';

            if ( *p_filterList == '\0' )
                break;

            filter = gtk_file_filter_new();            
        }

        if ( !NFDi_IsFilterSegmentChar( *p_filterList ) )
        {
            *p_typebuf = *p_filterList;
            p_typebuf++;
        }

        p_filterList++;
    }
    
    /* always append a wildcard option to the end*/

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name( filter, "*.*" );
    gtk_file_filter_add_pattern( filter, "*" );
    g_list_store_append( filters, filter );
    g_object_unref( filter );

    gtk_file_dialog_set_filters( dialog, G_LIST_MODEL(filters) );
    g_object_unref( filters );
}

static void SetDefaultPath( GtkFileDialog *dialog, const char *defaultPath )
{
    if ( !defaultPath || strlen(defaultPath) == 0 )
        return;

    GFile *folder = g_file_new_for_path(defaultPath);
    gtk_file_dialog_set_initial_folder( dialog, folder );
    g_object_unref(folder);
}

static void WaitForCleanup(void)
{
    while (g_main_context_pending(NULL))
        g_main_context_iteration(NULL, FALSE);
}
                                 
/* public */

nfdresult_t NFD_OpenDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            nfdchar_t **outPath )
{    
    GtkFileDialog *dialog;
    struct DialogData data;

    if ( !gtk_init_check() )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return NFD_ERROR;
    }

    dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Open File");

    /* Build the filter list */
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    data.result = NFD_CANCEL;
    data.outPath = outPath;
    data.outPaths = NULL;
    data.loop = g_main_loop_new(NULL, FALSE);

    gtk_file_dialog_open(dialog, NULL, NULL, OnOpenResponse, &data);
    
    g_main_loop_run(data.loop);

    g_main_loop_unref(data.loop);
    g_object_unref(dialog);

    WaitForCleanup();

    return data.result;
}


nfdresult_t NFD_OpenDialogMultiple( const nfdchar_t *filterList,
                                    const nfdchar_t *defaultPath,
                                    nfdpathset_t *outPaths )
{
    GtkFileDialog *dialog;
    struct DialogData data;

    if ( !gtk_init_check() )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return NFD_ERROR;
    }

    dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Open Files");

    /* Build the filter list */
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);

    data.result = NFD_CANCEL;
    data.outPath = NULL;
    data.outPaths = outPaths;
    data.loop = g_main_loop_new(NULL, FALSE);

    gtk_file_dialog_open_multiple(dialog, NULL, NULL, OnOpenMultipleResponse, &data);
    
    g_main_loop_run(data.loop);

    g_main_loop_unref(data.loop);
    g_object_unref(dialog);

    WaitForCleanup();

    return data.result;
}

nfdresult_t NFD_SaveDialog( const nfdchar_t *filterList,
                            const nfdchar_t *defaultPath,
                            nfdchar_t **outPath )
{
    GtkFileDialog *dialog;
    struct DialogData data;

    if ( !gtk_init_check() )
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return NFD_ERROR;
    }

    dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Save File");

    /* Build the filter list */    
    AddFiltersToDialog(dialog, filterList);

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);
    
    data.result = NFD_CANCEL;
    data.outPath = outPath;
    data.outPaths = NULL;
    data.loop = g_main_loop_new(NULL, FALSE);

    gtk_file_dialog_save(dialog, NULL, NULL, OnSaveResponse, &data);
    
    g_main_loop_run(data.loop);

    g_main_loop_unref(data.loop);
    g_object_unref(dialog);

    WaitForCleanup();
    
    return data.result;
}

nfdresult_t NFD_PickFolder(const nfdchar_t *defaultPath,
    nfdchar_t **outPath)
{
    GtkFileDialog *dialog;
    struct DialogData data;

    if (!gtk_init_check())
    {
        NFDi_SetError(INIT_FAIL_MSG);
        return NFD_ERROR;
    }

    dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, "Select folder");

    /* Set the default path */
    SetDefaultPath(dialog, defaultPath);
    
    data.result = NFD_CANCEL;
    data.outPath = outPath;
    data.outPaths = NULL;
    data.loop = g_main_loop_new(NULL, FALSE);

    gtk_file_dialog_select_folder(dialog, NULL, NULL, OnFolderResponse, &data);
    
    g_main_loop_run(data.loop);

    g_main_loop_unref(data.loop);
    g_object_unref(dialog);

    WaitForCleanup();
    
    return data.result;
}
