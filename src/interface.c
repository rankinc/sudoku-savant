/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_sudoku_window (void)
{
  GtkWidget *sudoku_window;
  GtkWidget *table1;
  GtkWidget *menubar1;
  GtkWidget *menuitem1;
  GtkWidget *menuitem1_menu;
  GtkWidget *open1;
  GtkWidget *save_play1;
  GtkWidget *image37;
  GtkWidget *save_play_as1;
  GtkWidget *image38;
  GtkWidget *separator1;
  GtkWidget *save_puzzle1;
  GtkWidget *image39;
  GtkWidget *separatormenuitem1;
  GtkWidget *quit1;
  GtkWidget *menuitem2;
  GtkWidget *menuitem2_menu;
  GtkWidget *geometry1;
  GtkWidget *menuitem3;
  GtkWidget *menuitem3_menu;
  GtkWidget *show_logs1;
  GtkWidget *hilight_wrong_moves1;
  GtkWidget *menuitem4;
  GtkWidget *menuitem4_menu;
  GtkWidget *about1;
  GtkWidget *image40;
  GtkWidget *table2;
  GtkWidget *log_book;
  GtkWidget *hint_scroll;
  GtkWidget *hint_view;
  GtkWidget *hint_label;
  GtkWidget *solution_box;
  GtkWidget *solution_scroll;
  GtkWidget *solution_view;
  GtkWidget *solution_buttonbox;
  GtkWidget *reveal_button;
  GtkWidget *alignment11;
  GtkWidget *hbox1;
  GtkWidget *reveal_image;
  GtkWidget *reveal_label;
  GtkWidget *hide_button;
  GtkWidget *alignment12;
  GtkWidget *hbox2;
  GtkWidget *hide_image;
  GtkWidget *hide_label;
  GtkWidget *showall_button;
  GtkWidget *hideall_button;
  GtkWidget *auto_label;
  GtkWidget *vbox3;
  GtkWidget *toolbar_align;
  GtkWidget *hbox3;
  GtkWidget *board_frame;
  GtkWidget *canvas_alignment;
  GtkWidget *label1;
  GtkWidget *vbox1;
  GtkWidget *generate_frame;
  GtkWidget *gen_alignment;
  GtkWidget *vbuttonbox1;
  GtkWidget *auto_button;
  GtkWidget *manual_button;
  GtkWidget *check_button;
  GtkWidget *generate_label;
  GtkWidget *solve_frame;
  GtkWidget *solve_alignment;
  GtkWidget *control_table;
  GtkWidget *solve_box;
  GtkWidget *solve_button;
  GtkWidget *pad_align;
  GtkWidget *hbox4;
  GtkWidget *back_button;
  GtkWidget *image45;
  GtkWidget *forward_button;
  GtkWidget *image44;
  GtkWidget *eventbox1;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  accel_group = gtk_accel_group_new ();

  sudoku_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (sudoku_window), _("Sudoku Savant"));
  gtk_window_set_position (GTK_WINDOW (sudoku_window), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (sudoku_window), FALSE);

  table1 = gtk_table_new (3, 1, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (sudoku_window), table1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_show (menubar1);
  gtk_table_attach (GTK_TABLE (table1), menubar1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  menuitem1 = gtk_menu_item_new_with_mnemonic (_("_File"));
  gtk_widget_show (menuitem1);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem1);

  menuitem1_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menuitem1_menu);

  open1 = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
  gtk_widget_show (open1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), open1);

  save_play1 = gtk_image_menu_item_new_with_mnemonic (_("_Save Play"));
  gtk_widget_show (save_play1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), save_play1);
  gtk_widget_set_sensitive (save_play1, FALSE);

  image37 = gtk_image_new_from_stock ("gtk-save", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image37);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (save_play1), image37);

  save_play_as1 = gtk_image_menu_item_new_with_mnemonic (_("Save Play _As"));
  gtk_widget_show (save_play_as1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), save_play_as1);
  gtk_widget_set_sensitive (save_play_as1, FALSE);

  image38 = gtk_image_new_from_stock ("gtk-save-as", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image38);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (save_play_as1), image38);

  separator1 = gtk_separator_menu_item_new ();
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), separator1);
  gtk_widget_set_sensitive (separator1, FALSE);

  save_puzzle1 = gtk_image_menu_item_new_with_mnemonic (_("Save _Puzzle"));
  gtk_widget_show (save_puzzle1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), save_puzzle1);
  gtk_widget_set_sensitive (save_puzzle1, FALSE);

  image39 = gtk_image_new_from_stock ("gtk-save-as", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image39);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (save_puzzle1), image39);

  separatormenuitem1 = gtk_separator_menu_item_new ();
  gtk_widget_show (separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  quit1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  gtk_widget_show (quit1);
  gtk_container_add (GTK_CONTAINER (menuitem1_menu), quit1);

  menuitem2 = gtk_menu_item_new_with_mnemonic (_("_Options"));
  gtk_widget_show (menuitem2);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem2);

  menuitem2_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem2), menuitem2_menu);

  geometry1 = gtk_menu_item_new_with_mnemonic (_("_Geometry"));
  gtk_widget_show (geometry1);
  gtk_container_add (GTK_CONTAINER (menuitem2_menu), geometry1);

  menuitem3 = gtk_menu_item_new_with_mnemonic (_("_View"));
  gtk_widget_show (menuitem3);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem3);

  menuitem3_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem3), menuitem3_menu);

  show_logs1 = gtk_check_menu_item_new_with_mnemonic (_("Show _Logs"));
  gtk_widget_show (show_logs1);
  gtk_container_add (GTK_CONTAINER (menuitem3_menu), show_logs1);
  gtk_tooltips_set_tip (tooltips, show_logs1, _("Show the hint and solution logs."), NULL);

  hilight_wrong_moves1 = gtk_check_menu_item_new_with_mnemonic (_("_Highlight Wrong Moves"));
  gtk_widget_show (hilight_wrong_moves1);
  gtk_container_add (GTK_CONTAINER (menuitem3_menu), hilight_wrong_moves1);
  gtk_widget_set_sensitive (hilight_wrong_moves1, FALSE);
  gtk_tooltips_set_tip (tooltips, hilight_wrong_moves1, _("Highlight any cells where the number is incorrect."), NULL);

  menuitem4 = gtk_menu_item_new_with_mnemonic (_("_Help"));
  gtk_widget_show (menuitem4);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem4);

  menuitem4_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem4), menuitem4_menu);

  about1 = gtk_image_menu_item_new_with_mnemonic (_("_About"));
  gtk_widget_show (about1);
  gtk_container_add (GTK_CONTAINER (menuitem4_menu), about1);

  image40 = gtk_image_new_from_stock ("gtk-about", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image40);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (about1), image40);

  table2 = gtk_table_new (1, 2, FALSE);
  gtk_widget_show (table2);
  gtk_table_attach (GTK_TABLE (table1), table2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);

  log_book = gtk_notebook_new ();
  gtk_table_attach (GTK_TABLE (table2), log_book, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hint_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (hint_scroll);
  gtk_container_add (GTK_CONTAINER (log_book), hint_scroll);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (hint_scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (hint_scroll), GTK_SHADOW_ETCHED_IN);

  hint_view = gtk_text_view_new ();
  gtk_widget_show (hint_view);
  gtk_container_add (GTK_CONTAINER (hint_scroll), hint_view);
  gtk_widget_set_size_request (hint_view, 600, -1);
  GTK_WIDGET_UNSET_FLAGS (hint_view, GTK_CAN_FOCUS);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (hint_view), FALSE);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (hint_view), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (hint_view), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (hint_view), FALSE);

  hint_label = gtk_label_new_with_mnemonic (_("Hints"));
  gtk_widget_show (hint_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (log_book), gtk_notebook_get_nth_page (GTK_NOTEBOOK (log_book), 0), hint_label);
  gtk_label_set_use_markup (GTK_LABEL (hint_label), TRUE);

  solution_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (solution_box);
  gtk_container_add (GTK_CONTAINER (log_book), solution_box);

  solution_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (solution_scroll);
  gtk_box_pack_start (GTK_BOX (solution_box), solution_scroll, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (solution_scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (solution_scroll), GTK_SHADOW_ETCHED_IN);

  solution_view = gtk_text_view_new ();
  gtk_widget_show (solution_view);
  gtk_container_add (GTK_CONTAINER (solution_scroll), solution_view);
  gtk_widget_set_size_request (solution_view, 420, -1);
  GTK_WIDGET_UNSET_FLAGS (solution_view, GTK_CAN_FOCUS);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (solution_view), FALSE);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (solution_view), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (solution_view), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (solution_view), FALSE);

  solution_buttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (solution_buttonbox);
  gtk_box_pack_start (GTK_BOX (solution_box), solution_buttonbox, FALSE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (solution_buttonbox), 4);
  gtk_widget_set_sensitive (solution_buttonbox, FALSE);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (solution_buttonbox), GTK_BUTTONBOX_SPREAD);

  reveal_button = gtk_button_new ();
  gtk_widget_show (reveal_button);
  gtk_container_add (GTK_CONTAINER (solution_buttonbox), reveal_button);
  gtk_widget_set_sensitive (reveal_button, FALSE);
  GTK_WIDGET_SET_FLAGS (reveal_button, GTK_CAN_DEFAULT);

  alignment11 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment11);
  gtk_container_add (GTK_CONTAINER (reveal_button), alignment11);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment11), hbox1);

  reveal_image = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (reveal_image);
  gtk_box_pack_start (GTK_BOX (hbox1), reveal_image, FALSE, FALSE, 0);

  reveal_label = gtk_label_new_with_mnemonic (_("_Reveal"));
  gtk_widget_show (reveal_label);
  gtk_box_pack_start (GTK_BOX (hbox1), reveal_label, FALSE, FALSE, 0);

  hide_button = gtk_button_new ();
  gtk_widget_show (hide_button);
  gtk_container_add (GTK_CONTAINER (solution_buttonbox), hide_button);
  gtk_widget_set_sensitive (hide_button, FALSE);
  GTK_WIDGET_SET_FLAGS (hide_button, GTK_CAN_DEFAULT);

  alignment12 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment12);
  gtk_container_add (GTK_CONTAINER (hide_button), alignment12);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment12), hbox2);

  hide_image = gtk_image_new_from_stock ("gtk-remove", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (hide_image);
  gtk_box_pack_start (GTK_BOX (hbox2), hide_image, FALSE, FALSE, 0);

  hide_label = gtk_label_new_with_mnemonic (_("Hi_de"));
  gtk_widget_show (hide_label);
  gtk_box_pack_start (GTK_BOX (hbox2), hide_label, FALSE, FALSE, 0);

  showall_button = gtk_button_new_with_mnemonic (_("Show _All"));
  gtk_widget_show (showall_button);
  gtk_container_add (GTK_CONTAINER (solution_buttonbox), showall_button);
  gtk_widget_set_sensitive (showall_button, FALSE);
  GTK_WIDGET_SET_FLAGS (showall_button, GTK_CAN_DEFAULT);

  hideall_button = gtk_button_new_with_mnemonic (_("Hid_e All"));
  gtk_widget_show (hideall_button);
  gtk_container_add (GTK_CONTAINER (solution_buttonbox), hideall_button);
  gtk_widget_set_sensitive (hideall_button, FALSE);
  GTK_WIDGET_SET_FLAGS (hideall_button, GTK_CAN_DEFAULT);

  auto_label = gtk_label_new_with_mnemonic (_("Computer's Solution"));
  gtk_widget_show (auto_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (log_book), gtk_notebook_get_nth_page (GTK_NOTEBOOK (log_book), 1), auto_label);
  gtk_label_set_use_markup (GTK_LABEL (auto_label), TRUE);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_table_attach (GTK_TABLE (table2), vbox3, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL), 0, 0);

  toolbar_align = gtk_alignment_new (0.5, 0, 1, 1);
  gtk_widget_show (toolbar_align);
  gtk_box_pack_start (GTK_BOX (vbox3), toolbar_align, TRUE, TRUE, 0);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox3, TRUE, TRUE, 0);

  board_frame = gtk_aspect_frame_new (NULL, 0, 0.5, 1, TRUE);
  gtk_widget_show (board_frame);
  gtk_box_pack_start (GTK_BOX (hbox3), board_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (board_frame), 4);

  canvas_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (canvas_alignment);
  gtk_container_add (GTK_CONTAINER (board_frame), canvas_alignment);
  gtk_alignment_set_padding (GTK_ALIGNMENT (canvas_alignment), 0, 0, 12, 0);

  label1 = gtk_label_new_with_mnemonic (_("<b>Unknown</b>"));
  gtk_widget_show (label1);
  gtk_frame_set_label_widget (GTK_FRAME (board_frame), label1);
  gtk_label_set_use_markup (GTK_LABEL (label1), TRUE);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox1, TRUE, TRUE, 0);

  generate_frame = gtk_frame_new (NULL);
  gtk_widget_show (generate_frame);
  gtk_box_pack_start (GTK_BOX (vbox1), generate_frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (generate_frame), 4);
  gtk_frame_set_label_align (GTK_FRAME (generate_frame), 0.5, 0.5);

  gen_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (gen_alignment);
  gtk_container_add (GTK_CONTAINER (generate_frame), gen_alignment);

  vbuttonbox1 = gtk_vbutton_box_new ();
  gtk_widget_show (vbuttonbox1);
  gtk_container_add (GTK_CONTAINER (gen_alignment), vbuttonbox1);

  auto_button = gtk_button_new_with_mnemonic (_("A_uto"));
  gtk_widget_show (auto_button);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), auto_button);
  gtk_container_set_border_width (GTK_CONTAINER (auto_button), 2);
  GTK_WIDGET_SET_FLAGS (auto_button, GTK_CAN_DEFAULT);

  manual_button = gtk_button_new_with_mnemonic (_("_Manual"));
  gtk_widget_show (manual_button);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), manual_button);
  gtk_container_set_border_width (GTK_CONTAINER (manual_button), 2);
  GTK_WIDGET_SET_FLAGS (manual_button, GTK_CAN_DEFAULT);

  check_button = gtk_button_new_with_mnemonic (_("_Check"));
  gtk_widget_show (check_button);
  gtk_container_add (GTK_CONTAINER (vbuttonbox1), check_button);
  gtk_container_set_border_width (GTK_CONTAINER (check_button), 2);
  gtk_widget_set_sensitive (check_button, FALSE);
  GTK_WIDGET_SET_FLAGS (check_button, GTK_CAN_DEFAULT);

  generate_label = gtk_label_new (_("Generate"));
  gtk_widget_show (generate_label);
  gtk_frame_set_label_widget (GTK_FRAME (generate_frame), generate_label);
  gtk_label_set_use_markup (GTK_LABEL (generate_label), TRUE);
  gtk_label_set_justify (GTK_LABEL (generate_label), GTK_JUSTIFY_CENTER);

  solve_frame = gtk_frame_new (NULL);
  gtk_widget_show (solve_frame);
  gtk_box_pack_start (GTK_BOX (vbox1), solve_frame, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (solve_frame), 4);
  gtk_frame_set_label_align (GTK_FRAME (solve_frame), 0.5, 0.5);

  solve_alignment = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (solve_alignment);
  gtk_container_add (GTK_CONTAINER (solve_frame), solve_alignment);

  control_table = gtk_table_new (3, 1, FALSE);
  gtk_widget_show (control_table);
  gtk_container_add (GTK_CONTAINER (solve_alignment), control_table);

  solve_box = gtk_vbutton_box_new ();
  gtk_widget_show (solve_box);
  gtk_table_attach (GTK_TABLE (control_table), solve_box, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  solve_button = gtk_button_new_with_mnemonic (_("Solve"));
  gtk_widget_show (solve_button);
  gtk_container_add (GTK_CONTAINER (solve_box), solve_button);
  gtk_container_set_border_width (GTK_CONTAINER (solve_button), 2);
  gtk_widget_set_sensitive (solve_button, FALSE);
  GTK_WIDGET_SET_FLAGS (solve_button, GTK_CAN_DEFAULT);

  pad_align = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (pad_align);
  gtk_table_attach (GTK_TABLE (control_table), pad_align, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_SHRINK),
                    (GtkAttachOptions) (0), 0, 2);
  gtk_container_set_border_width (GTK_CONTAINER (pad_align), 2);

  hbox4 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox4);
  gtk_table_attach (GTK_TABLE (control_table), hbox4, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 1);

  back_button = gtk_button_new ();
  gtk_widget_show (back_button);
  gtk_box_pack_start (GTK_BOX (hbox4), back_button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (back_button, FALSE);
  GTK_WIDGET_SET_FLAGS (back_button, GTK_CAN_DEFAULT);
  gtk_button_set_relief (GTK_BUTTON (back_button), GTK_RELIEF_NONE);

  image45 = gtk_image_new_from_stock ("gtk-go-back", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image45);
  gtk_container_add (GTK_CONTAINER (back_button), image45);

  forward_button = gtk_button_new ();
  gtk_widget_show (forward_button);
  gtk_box_pack_start (GTK_BOX (hbox4), forward_button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (forward_button, FALSE);
  GTK_WIDGET_SET_FLAGS (forward_button, GTK_CAN_DEFAULT);
  gtk_button_set_relief (GTK_BUTTON (forward_button), GTK_RELIEF_NONE);

  image44 = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image44);
  gtk_container_add (GTK_CONTAINER (forward_button), image44);

  eventbox1 = gtk_event_box_new ();
  gtk_widget_show (eventbox1);
  gtk_table_attach (GTK_TABLE (table1), eventbox1, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  gtk_container_add (GTK_CONTAINER (eventbox1), statusbar1);
  gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR (statusbar1), FALSE);

  g_signal_connect ((gpointer) sudoku_window, "destroy",
                    G_CALLBACK (gtk_main_quit),
                    NULL);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label1), log_book);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (sudoku_window, sudoku_window, "sudoku_window");
  GLADE_HOOKUP_OBJECT (sudoku_window, table1, "table1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menubar1, "menubar1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem1, "menuitem1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem1_menu, "menuitem1_menu");
  GLADE_HOOKUP_OBJECT (sudoku_window, open1, "open1");
  GLADE_HOOKUP_OBJECT (sudoku_window, save_play1, "save_play1");
  GLADE_HOOKUP_OBJECT (sudoku_window, image37, "image37");
  GLADE_HOOKUP_OBJECT (sudoku_window, save_play_as1, "save_play_as1");
  GLADE_HOOKUP_OBJECT (sudoku_window, image38, "image38");
  GLADE_HOOKUP_OBJECT (sudoku_window, separator1, "separator1");
  GLADE_HOOKUP_OBJECT (sudoku_window, save_puzzle1, "save_puzzle1");
  GLADE_HOOKUP_OBJECT (sudoku_window, image39, "image39");
  GLADE_HOOKUP_OBJECT (sudoku_window, separatormenuitem1, "separatormenuitem1");
  GLADE_HOOKUP_OBJECT (sudoku_window, quit1, "quit1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem2, "menuitem2");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem2_menu, "menuitem2_menu");
  GLADE_HOOKUP_OBJECT (sudoku_window, geometry1, "geometry1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem3, "menuitem3");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem3_menu, "menuitem3_menu");
  GLADE_HOOKUP_OBJECT (sudoku_window, show_logs1, "show_logs1");
  GLADE_HOOKUP_OBJECT (sudoku_window, hilight_wrong_moves1, "hilight_wrong_moves1");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem4, "menuitem4");
  GLADE_HOOKUP_OBJECT (sudoku_window, menuitem4_menu, "menuitem4_menu");
  GLADE_HOOKUP_OBJECT (sudoku_window, about1, "about1");
  GLADE_HOOKUP_OBJECT (sudoku_window, image40, "image40");
  GLADE_HOOKUP_OBJECT (sudoku_window, table2, "table2");
  GLADE_HOOKUP_OBJECT (sudoku_window, log_book, "log_book");
  GLADE_HOOKUP_OBJECT (sudoku_window, hint_scroll, "hint_scroll");
  GLADE_HOOKUP_OBJECT (sudoku_window, hint_view, "hint_view");
  GLADE_HOOKUP_OBJECT (sudoku_window, hint_label, "hint_label");
  GLADE_HOOKUP_OBJECT (sudoku_window, solution_box, "solution_box");
  GLADE_HOOKUP_OBJECT (sudoku_window, solution_scroll, "solution_scroll");
  GLADE_HOOKUP_OBJECT (sudoku_window, solution_view, "solution_view");
  GLADE_HOOKUP_OBJECT (sudoku_window, solution_buttonbox, "solution_buttonbox");
  GLADE_HOOKUP_OBJECT (sudoku_window, reveal_button, "reveal_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, alignment11, "alignment11");
  GLADE_HOOKUP_OBJECT (sudoku_window, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (sudoku_window, reveal_image, "reveal_image");
  GLADE_HOOKUP_OBJECT (sudoku_window, reveal_label, "reveal_label");
  GLADE_HOOKUP_OBJECT (sudoku_window, hide_button, "hide_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, alignment12, "alignment12");
  GLADE_HOOKUP_OBJECT (sudoku_window, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (sudoku_window, hide_image, "hide_image");
  GLADE_HOOKUP_OBJECT (sudoku_window, hide_label, "hide_label");
  GLADE_HOOKUP_OBJECT (sudoku_window, showall_button, "showall_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, hideall_button, "hideall_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, auto_label, "auto_label");
  GLADE_HOOKUP_OBJECT (sudoku_window, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (sudoku_window, toolbar_align, "toolbar_align");
  GLADE_HOOKUP_OBJECT (sudoku_window, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (sudoku_window, board_frame, "board_frame");
  GLADE_HOOKUP_OBJECT (sudoku_window, canvas_alignment, "canvas_alignment");
  GLADE_HOOKUP_OBJECT (sudoku_window, label1, "label1");
  GLADE_HOOKUP_OBJECT (sudoku_window, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (sudoku_window, generate_frame, "generate_frame");
  GLADE_HOOKUP_OBJECT (sudoku_window, gen_alignment, "gen_alignment");
  GLADE_HOOKUP_OBJECT (sudoku_window, vbuttonbox1, "vbuttonbox1");
  GLADE_HOOKUP_OBJECT (sudoku_window, auto_button, "auto_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, manual_button, "manual_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, check_button, "check_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, generate_label, "generate_label");
  GLADE_HOOKUP_OBJECT (sudoku_window, solve_frame, "solve_frame");
  GLADE_HOOKUP_OBJECT (sudoku_window, solve_alignment, "solve_alignment");
  GLADE_HOOKUP_OBJECT (sudoku_window, control_table, "control_table");
  GLADE_HOOKUP_OBJECT (sudoku_window, solve_box, "solve_box");
  GLADE_HOOKUP_OBJECT (sudoku_window, solve_button, "solve_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, pad_align, "pad_align");
  GLADE_HOOKUP_OBJECT (sudoku_window, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (sudoku_window, back_button, "back_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, image45, "image45");
  GLADE_HOOKUP_OBJECT (sudoku_window, forward_button, "forward_button");
  GLADE_HOOKUP_OBJECT (sudoku_window, image44, "image44");
  GLADE_HOOKUP_OBJECT (sudoku_window, eventbox1, "eventbox1");
  GLADE_HOOKUP_OBJECT (sudoku_window, statusbar1, "statusbar1");
  GLADE_HOOKUP_OBJECT_NO_REF (sudoku_window, tooltips, "tooltips");

  gtk_window_add_accel_group (GTK_WINDOW (sudoku_window), accel_group);

  return sudoku_window;
}

GtkWidget*
create_filechooserdialog1 (void)
{
  GtkWidget *filechooserdialog1;
  GtkWidget *dialog_vbox1;
  GtkWidget *dialog_action_area1;
  GtkWidget *file_cancel;
  GtkWidget *file_open;
  GtkWidget *file_save;

  filechooserdialog1 = gtk_file_chooser_dialog_new ("", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, NULL);
  gtk_container_set_border_width (GTK_CONTAINER (filechooserdialog1), 5);
  gtk_window_set_type_hint (GTK_WINDOW (filechooserdialog1), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox1 = GTK_DIALOG (filechooserdialog1)->vbox;
  gtk_widget_show (dialog_vbox1);

  dialog_action_area1 = GTK_DIALOG (filechooserdialog1)->action_area;
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);

  file_cancel = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (file_cancel);
  gtk_dialog_add_action_widget (GTK_DIALOG (filechooserdialog1), file_cancel, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (file_cancel, GTK_CAN_DEFAULT);

  file_open = gtk_button_new_from_stock ("gtk-open");
  gtk_dialog_add_action_widget (GTK_DIALOG (filechooserdialog1), file_open, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (file_open, GTK_CAN_DEFAULT);

  file_save = gtk_button_new_from_stock ("gtk-save");
  gtk_dialog_add_action_widget (GTK_DIALOG (filechooserdialog1), file_save, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (file_save, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (filechooserdialog1, filechooserdialog1, "filechooserdialog1");
  GLADE_HOOKUP_OBJECT_NO_REF (filechooserdialog1, dialog_vbox1, "dialog_vbox1");
  GLADE_HOOKUP_OBJECT_NO_REF (filechooserdialog1, dialog_action_area1, "dialog_action_area1");
  GLADE_HOOKUP_OBJECT (filechooserdialog1, file_cancel, "file_cancel");
  GLADE_HOOKUP_OBJECT (filechooserdialog1, file_open, "file_open");
  GLADE_HOOKUP_OBJECT (filechooserdialog1, file_save, "file_save");

  gtk_widget_grab_default (file_open);
  return filechooserdialog1;
}

GtkWidget*
create_geometry_dialog (void)
{
  GtkWidget *geometry_dialog;
  GtkWidget *dialog_vbox2;
  GtkWidget *geometry_frame;
  GtkWidget *alignment5;
  GtkWidget *geobox;
  GtkObject *width_button_adj;
  GtkWidget *width_button;
  GtkWidget *times;
  GtkObject *height_button_adj;
  GtkWidget *height_button;
  GtkWidget *geolabel;
  GtkWidget *dialog_action_area2;
  GtkWidget *cancelbutton1;
  GtkWidget *okbutton1;

  geometry_dialog = gtk_dialog_new ();
  gtk_container_set_border_width (GTK_CONTAINER (geometry_dialog), 2);
  gtk_window_set_title (GTK_WINDOW (geometry_dialog), _("Geometry"));
  gtk_window_set_resizable (GTK_WINDOW (geometry_dialog), FALSE);
  gtk_window_set_type_hint (GTK_WINDOW (geometry_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox2 = GTK_DIALOG (geometry_dialog)->vbox;
  gtk_widget_show (dialog_vbox2);

  geometry_frame = gtk_frame_new (NULL);
  gtk_widget_show (geometry_frame);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), geometry_frame, FALSE, FALSE, 0);

  alignment5 = gtk_alignment_new (0.5, 0, 1, 1);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (geometry_frame), alignment5);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment5), 0, 0, 24, 24);

  geobox = gtk_hbox_new (FALSE, 4);
  gtk_widget_show (geobox);
  gtk_container_add (GTK_CONTAINER (alignment5), geobox);

  width_button_adj = gtk_adjustment_new (3, 2, 15, 1, 10, 0);
  width_button = gtk_spin_button_new (GTK_ADJUSTMENT (width_button_adj), 1, 0);
  gtk_widget_show (width_button);
  gtk_box_pack_start (GTK_BOX (geobox), width_button, FALSE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (width_button), TRUE);

  times = gtk_label_new (_("x"));
  gtk_widget_show (times);
  gtk_box_pack_start (GTK_BOX (geobox), times, TRUE, TRUE, 0);

  height_button_adj = gtk_adjustment_new (3, 2, 15, 1, 10, 0);
  height_button = gtk_spin_button_new (GTK_ADJUSTMENT (height_button_adj), 1, 0);
  gtk_widget_show (height_button);
  gtk_box_pack_start (GTK_BOX (geobox), height_button, FALSE, TRUE, 0);
  gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (height_button), TRUE);

  geolabel = gtk_label_new (_("<b>Width x Height</b>"));
  gtk_widget_show (geolabel);
  gtk_frame_set_label_widget (GTK_FRAME (geometry_frame), geolabel);
  gtk_label_set_use_markup (GTK_LABEL (geolabel), TRUE);

  dialog_action_area2 = GTK_DIALOG (geometry_dialog)->action_area;
  gtk_widget_show (dialog_action_area2);

  cancelbutton1 = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancelbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (geometry_dialog), cancelbutton1, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancelbutton1, GTK_CAN_DEFAULT);

  okbutton1 = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (okbutton1);
  gtk_dialog_add_action_widget (GTK_DIALOG (geometry_dialog), okbutton1, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (okbutton1, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (geometry_dialog, geometry_dialog, "geometry_dialog");
  GLADE_HOOKUP_OBJECT_NO_REF (geometry_dialog, dialog_vbox2, "dialog_vbox2");
  GLADE_HOOKUP_OBJECT (geometry_dialog, geometry_frame, "geometry_frame");
  GLADE_HOOKUP_OBJECT (geometry_dialog, alignment5, "alignment5");
  GLADE_HOOKUP_OBJECT (geometry_dialog, geobox, "geobox");
  GLADE_HOOKUP_OBJECT (geometry_dialog, width_button, "width_button");
  GLADE_HOOKUP_OBJECT (geometry_dialog, times, "times");
  GLADE_HOOKUP_OBJECT (geometry_dialog, height_button, "height_button");
  GLADE_HOOKUP_OBJECT (geometry_dialog, geolabel, "geolabel");
  GLADE_HOOKUP_OBJECT_NO_REF (geometry_dialog, dialog_action_area2, "dialog_action_area2");
  GLADE_HOOKUP_OBJECT (geometry_dialog, cancelbutton1, "cancelbutton1");
  GLADE_HOOKUP_OBJECT (geometry_dialog, okbutton1, "okbutton1");

  return geometry_dialog;
}

GtkWidget*
create_aboutbox (void)
{
  GtkWidget *aboutbox;
  GtkWidget *vbox2;
  GtkWidget *title_label;
  GtkWidget *about_frame;
  GtkWidget *alignment13;
  GtkWidget *about_scroll;
  GtkWidget *about_view;
  GtkWidget *about_buttonbox;
  GtkWidget *about_button;

  aboutbox = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (aboutbox), _("About Sudoku Savant"));
  gtk_window_set_position (GTK_WINDOW (aboutbox), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (aboutbox), 340, 290);
  gtk_window_set_gravity (GTK_WINDOW (aboutbox), GDK_GRAVITY_CENTER);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (aboutbox), vbox2);

  title_label = gtk_label_new ("");
  gtk_widget_show (title_label);
  gtk_box_pack_start (GTK_BOX (vbox2), title_label, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (title_label), TRUE);
  gtk_label_set_justify (GTK_LABEL (title_label), GTK_JUSTIFY_CENTER);

  about_frame = gtk_frame_new (NULL);
  gtk_widget_show (about_frame);
  gtk_box_pack_start (GTK_BOX (vbox2), about_frame, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (about_frame), 4);

  alignment13 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment13);
  gtk_container_add (GTK_CONTAINER (about_frame), alignment13);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment13), 8, 8, 8, 8);

  about_scroll = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (about_scroll);
  gtk_container_add (GTK_CONTAINER (alignment13), about_scroll);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (about_scroll), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (about_scroll), GTK_SHADOW_ETCHED_IN);

  about_view = gtk_text_view_new ();
  gtk_widget_show (about_view);
  gtk_container_add (GTK_CONTAINER (about_scroll), about_view);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (about_view), FALSE);
  gtk_text_view_set_accepts_tab (GTK_TEXT_VIEW (about_view), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (about_view), GTK_WRAP_WORD);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (about_view), FALSE);

  about_buttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (about_buttonbox);
  gtk_box_pack_end (GTK_BOX (vbox2), about_buttonbox, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (about_buttonbox), 4);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (about_buttonbox), GTK_BUTTONBOX_SPREAD);

  about_button = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (about_button);
  gtk_container_add (GTK_CONTAINER (about_buttonbox), about_button);
  GTK_WIDGET_SET_FLAGS (about_button, GTK_CAN_DEFAULT);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (aboutbox, aboutbox, "aboutbox");
  GLADE_HOOKUP_OBJECT (aboutbox, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (aboutbox, title_label, "title_label");
  GLADE_HOOKUP_OBJECT (aboutbox, about_frame, "about_frame");
  GLADE_HOOKUP_OBJECT (aboutbox, alignment13, "alignment13");
  GLADE_HOOKUP_OBJECT (aboutbox, about_scroll, "about_scroll");
  GLADE_HOOKUP_OBJECT (aboutbox, about_view, "about_view");
  GLADE_HOOKUP_OBJECT (aboutbox, about_buttonbox, "about_buttonbox");
  GLADE_HOOKUP_OBJECT (aboutbox, about_button, "about_button");

  return aboutbox;
}

