/* gr-window.c
 *
 * Copyright (C) 2016 Matthias Clasen <mclasen@redhat.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <glib/gi18n.h>
#include "gr-window.h"
#include "gr-details-page.h"
#include "gr-edit-page.h"
#include "gr-list-page.h"
#include "gr-cuisine-page.h"
#include "gr-search-page.h"

struct _GrWindow
{
        GtkApplicationWindow parent_instance;

        GtkWidget *search_button;
        GtkWidget *search_bar;
        GtkWidget *search_entry;
        GtkWidget *header_stack;
        GtkWidget *main_stack;
        GtkWidget *details_header;
        GtkWidget *details_page;
        GtkWidget *edit_header;
        GtkWidget *edit_page;
        GtkWidget *list_header;
        GtkWidget *list_page;
        GtkWidget *search_header;
        GtkWidget *search_button2;
        GtkWidget *search_page;
        GtkWidget *cuisines_page;
        GtkWidget *cuisine_header;
        GtkWidget *cuisine_page;
};

G_DEFINE_TYPE (GrWindow, gr_window, GTK_TYPE_APPLICATION_WINDOW)

static void
back_to_main (GrWindow *window)
{
        gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (window->search_bar), FALSE);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "recipes");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "recipes");
}

static void
back_to_cuisines (GrWindow *window)
{
        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "recipes");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "cuisines");
}

static void
new_recipe (GrWindow *window)
{
        gr_edit_page_clear (GR_EDIT_PAGE (window->edit_page));

        gtk_header_bar_set_title (GTK_HEADER_BAR (window->edit_header), "Add a new recipe");

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "edit");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "edit");
}

static void
add_recipe (GrWindow *window)
{
        if (gr_edit_page_save (GR_EDIT_PAGE (window->edit_page)))
                back_to_main (window);
}

static void
stop_search (GrWindow *window)
{
        back_to_main (window);
}

static void
maybe_stop_search (GtkButton *button, GParamSpec *pspec, GrWindow *window)
{
        if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)))
                stop_search (window);
}

static void
search_changed (GrWindow *window)
{
        const char *visible;

        visible = gtk_stack_get_visible_child_name (GTK_STACK (window->main_stack));

        if (strcmp (visible, "search") != 0) {
                gtk_header_bar_set_title (GTK_HEADER_BAR (window->search_header), _("Search"));
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (window->search_button2), TRUE);
                gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "search");
                gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "search");
        }

        gr_search_page_update_search (GR_SEARCH_PAGE (window->search_page),
                                      gtk_entry_get_text (GTK_ENTRY (window->search_entry)));
}

GrWindow *
gr_window_new (GrApp *app)
{
        return g_object_new (GR_TYPE_WINDOW, "application", app, NULL);
}

static void
gr_window_finalize (GObject *object)
{
        GrWindow *self = (GrWindow *)object;

        G_OBJECT_CLASS (gr_window_parent_class)->finalize (object);
}

static void
gr_window_class_init (GrWindowClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->finalize = gr_window_finalize;

        gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (klass),
                                                     "/org/gnome/Recipes/gr-window.ui");

        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_button);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_bar);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_entry);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, header_stack);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, main_stack);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, details_header);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, details_page);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, edit_header);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, edit_page);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, list_header);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, list_page);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_header);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_button2);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, search_page);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, cuisines_page);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, cuisine_header);
        gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (klass), GrWindow, cuisine_page);

        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (klass), new_recipe);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (klass), back_to_main);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (klass), add_recipe);
        gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (klass), back_to_cuisines);
}

static void
gr_window_init (GrWindow *self)
{
        gtk_widget_init_template (GTK_WIDGET (self));

        g_object_bind_property (self->search_button, "active",
                                self->search_bar, "search-mode-enabled",
                                G_BINDING_BIDIRECTIONAL);

        g_signal_connect_swapped (self->search_entry, "search-changed", G_CALLBACK (search_changed), self);
        g_signal_connect_swapped (self->search_entry, "stop-search", G_CALLBACK (stop_search), self);
        g_signal_connect (self->search_button2, "notify::active", G_CALLBACK (maybe_stop_search), self);
}

void
gr_window_show_recipe (GrWindow *window,
                       GrRecipe *recipe)
{
        g_autofree char *name = NULL;

        gr_details_page_set_recipe (GR_DETAILS_PAGE (window->details_page), recipe);

        g_object_get (recipe, "name", &name, NULL);
        gtk_header_bar_set_title (GTK_HEADER_BAR (window->details_header), name);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "details");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "details");
}

void
gr_window_show_main (GrWindow *window)
{
        back_to_main (window);
}

void
gr_window_edit_recipe (GrWindow *window,
                       GrRecipe *recipe)
{
        g_autofree char *name = NULL;

        gr_edit_page_edit (GR_EDIT_PAGE (window->edit_page), recipe);

        g_object_get (recipe, "name", &name, NULL);
        gtk_header_bar_set_title (GTK_HEADER_BAR (window->edit_header), name);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "edit");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "edit");
}

void
gr_window_show_diet (GrWindow   *window,
                     const char *title,
                     GrDiets     diet)
{
        gtk_header_bar_set_title (GTK_HEADER_BAR (window->list_header), title);
	gr_list_page_populate_from_diet (GR_LIST_PAGE (window->list_page), diet);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "list");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "list");
}

void
gr_window_show_chef (GrWindow *window,
                     GrAuthor *chef)
{
	g_autofree char *title = NULL;
	g_autofree char *name = NULL;

	g_object_get (chef, "name", &name, NULL);
	title = g_strdup_printf (_("Recipes by %s"), name);
        gtk_header_bar_set_title (GTK_HEADER_BAR (window->list_header), title);
	gr_list_page_populate_from_chef (GR_LIST_PAGE (window->list_page), chef);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "list");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "list");
}

void
gr_window_show_cuisine (GrWindow   *window,
                        const char *cuisine,
                        const char *title)
{
        gr_cuisine_page_set_cuisine (GR_CUISINE_PAGE (window->cuisine_page), cuisine);
        gtk_header_bar_set_title (GTK_HEADER_BAR (window->cuisine_header), title);

        gtk_stack_set_visible_child_name (GTK_STACK (window->header_stack), "cuisine");
        gtk_stack_set_visible_child_name (GTK_STACK (window->main_stack), "cuisine");
}

