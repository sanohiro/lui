// 11 june 2015
#include "uipriv_unix.h"

struct uiCombobox {
	uiUnixControl c;
	GtkWidget *widget;
	GtkComboBox *combobox;
	GtkComboBoxText *comboboxText;
	void (*onSelected)(uiCombobox *, void *);
	void *onSelectedData;
	gulong onSelectedSignal;
};

uiUnixControlAllDefaults(uiCombobox)

static void onChanged(GtkComboBox *cbox, gpointer data)
{
	uiCombobox *c = uiCombobox(data);

	(*(c->onSelected))(c, c->onSelectedData);
}

static void defaultOnSelected(uiCombobox *c, void *data)
{
	// do nothing
}

void uiComboboxAppend(uiCombobox *c, const char *text)
{
	gtk_combo_box_text_append(c->comboboxText, NULL, text);
}

void uiComboboxInsertAt(uiCombobox *c, int n, const char *text)
{
	gtk_combo_box_text_insert(c->comboboxText, n, NULL, text);
}

void uiComboboxDelete(uiCombobox *c, int n)
{
	g_signal_handler_block(c->combobox, c->onSelectedSignal);
	gtk_combo_box_text_remove(c->comboboxText, n);
	g_signal_handler_unblock(c->combobox, c->onSelectedSignal);
}

void uiComboboxClear(uiCombobox *c)
{
	g_signal_handler_block(c->combobox, c->onSelectedSignal);
	gtk_combo_box_text_remove_all(c->comboboxText);
	g_signal_handler_unblock(c->combobox, c->onSelectedSignal);
}

int uiComboboxNumItems(uiCombobox *c)
{
	GtkListStore *store;

	store = GTK_LIST_STORE(gtk_combo_box_get_model(c->combobox));
	return gtk_tree_model_iter_n_children(GTK_TREE_MODEL(store), NULL);
}

int uiComboboxSelected(uiCombobox *c)
{
	return gtk_combo_box_get_active(c->combobox);
}

void uiComboboxSetSelected(uiCombobox *c, int n)
{
	// we need to inhibit sending of ::changed because this WILL send a ::changed otherwise
	g_signal_handler_block(c->combobox, c->onSelectedSignal);
	gtk_combo_box_set_active(c->combobox, n);
	g_signal_handler_unblock(c->combobox, c->onSelectedSignal);
}

void uiComboboxOnSelected(uiCombobox *c, void (*f)(uiCombobox *c, void *data), void *data)
{
	c->onSelected = f;
	c->onSelectedData = data;
}

uiCombobox *uiNewCombobox(void)
{
	uiCombobox *c;

	uiUnixNewControl(uiCombobox, c);

	c->widget = gtk_combo_box_text_new();
	c->combobox = GTK_COMBO_BOX(c->widget);
	c->comboboxText = GTK_COMBO_BOX_TEXT(c->widget);

	c->onSelectedSignal = g_signal_connect(c->widget, "changed", G_CALLBACK(onChanged), c);
	uiComboboxOnSelected(c, defaultOnSelected, NULL);

	return c;
}
