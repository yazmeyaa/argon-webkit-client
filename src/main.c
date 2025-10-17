#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <gio/gio.h>
#include <JavaScriptCore/JavaScript.h>
#include <stdlib.h>
#include <string.h>

static void handle_argon_custom_uri_scheme(WebKitURISchemeRequest *request, gpointer user_data);
static gboolean on_button_press(GtkWidget *widget, const GdkEventButton *event, gpointer data);

int main(int argc, char *argv[]) {
    setenv("WEBKIT_DISABLE_COMPOSITING_MODE", "1", 1);
    gtk_init(&argc, &argv);

    char* g_resource_path = g_strdup_printf("%s/.argon_client/client.gresource", g_get_home_dir());
    GError *error = NULL;
    GResource *resource = g_resource_load(g_resource_path, &error);
    g_free(g_resource_path);

    if (!resource) {
        g_printerr("Failed to load resource: %s\n", error->message);
        g_error_free(error);
        return 1;
    }
    g_resources_register(resource);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1366, 768);
    gtk_window_set_title(GTK_WINDOW(window), "Argon chat");

    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(web_view, "argon:///index.html");

    WebKitSettings *settings = webkit_web_view_get_settings(web_view);
    webkit_settings_set_enable_developer_extras(settings, TRUE);

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
    gtk_widget_show_all(window);

    WebKitWebContext *context = webkit_web_view_get_context(web_view);
    webkit_web_context_set_cache_model(context, WEBKIT_CACHE_MODEL_DOCUMENT_BROWSER);

    webkit_web_context_register_uri_scheme(context, "argon",
                                           handle_argon_custom_uri_scheme,
                                           NULL, NULL);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(web_view, "button-press-event", G_CALLBACK(on_button_press), web_view);

    gtk_main();
    return 0;
}


static gboolean on_button_press(GtkWidget *widget, const GdkEventButton *event, const gpointer data) {
    (void)data;
    if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
        WebKitWebView *web_view = WEBKIT_WEB_VIEW(widget);
        webkit_web_inspector_show(webkit_web_view_get_inspector(web_view));
        return TRUE;
    }
    return FALSE;
}

static void handle_argon_custom_uri_scheme(WebKitURISchemeRequest *request, gpointer user_data) {
    (void)user_data;
    const gchar *uri = webkit_uri_scheme_request_get_uri(request);
    const gchar *path = webkit_uri_scheme_request_get_path(request);
    g_print("Custom scheme request: %s (path: %s)\n", uri, path);

    gchar *resource_path = g_strdup_printf("/argon_build%s", path);
    GError *error = NULL;
    GInputStream *stream = g_resources_open_stream(resource_path, G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    g_free(resource_path);

    if (!stream) {
        g_printerr("Resource not found: %s (error: %s)\n", path, error->message);
        webkit_uri_scheme_request_finish_error(request, error);
        g_error_free(error);
        return;
    }

    const char *mime_type = "application/octet-stream";
    if (g_str_has_suffix(path, ".html")) mime_type = "text/html";
    else if (g_str_has_suffix(path, ".js")) mime_type = "application/javascript";
    else if (g_str_has_suffix(path, ".css")) mime_type = "text/css";
    else if (g_str_has_suffix(path, ".png")) mime_type = "image/png";
    else if (g_str_has_suffix(path, ".jpg") || g_str_has_suffix(path, ".jpeg")) mime_type = "image/jpeg";
    else if (g_str_has_suffix(path, ".woff")) mime_type = "font/woff";
    else if (g_str_has_suffix(path, ".woff2")) mime_type = "font/woff2";

    webkit_uri_scheme_request_finish(request, stream, -1, mime_type);
    g_object_unref(stream);
}
