#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <ctype.h>
#include <Xm/MessageB.h>
#include <Xm/List.h>
#include <Xm/Form.h>
#include <Xm/Separator.h>

typedef struct UserNode {
   char name[100];
   char city[100];
   int age;
   struct UserNode *next;
} UserData;

typedef struct {
	  Widget name_field;
	  Widget city_field;
	  Widget age_field;
      Widget display_list;
      UserData **list_head;

      // nav containe refs
      Widget home_view_panel;
      Widget about_view_panel;

	} FormFields;



// Corrected XPointer to XtPointer and fixed typo in call_data
void button_clicked(Widget w, XtPointer client_data, XtPointer call_data){
    printf("clicked!\n");
}

int is_numeric(const char *str) {
    // 1. Check for an empty string
    if (*str == '\0') return 0;
    
    // 2. Loop through every character in the string
    while (*str) {
        // 3. Test if the current character is a digit
        if (!isdigit((unsigned char)*str)) return 0;
        
        // 4. Move pointer to the next character
        str++;
    }
    
    // 5. If loop completes, everything was a digit
    return 1;
}


void on_close_dialog(Widget w, XtPointer client_data, XtPointer call_data){
    Widget dialog = (Widget)client_data;
    XtUnmanageChild(dialog);
}

void on_submit(Widget w, XtPointer client_data, XtPointer call_data){
      FormFields *fields = (FormFields *)client_data;
      
      char *name = XmTextFieldGetString(fields->name_field);
      char *city = XmTextFieldGetString(fields->city_field);
      char *age_field = XmTextFieldGetString(fields->age_field);


      // dialog
      char message[256];
      snprintf(message, sizeof(message), "You submitted:%s\n", name);

      XmString motif_message = XmStringCreateLocalized(message);
      XmString title_string = XmStringCreateLocalized("Submission status");

      Widget dialog = XmCreateInformationDialog(fields->name_field, "infoDialog", NULL , 0);
      
       
      XtVaSetValues(dialog, XmNmessageString, motif_message,
       XmNdialogTitle, title_string, 
       XmNwidth, 500,
       XmNheight, 300,
       XmNresizePolicy, 
       XmRESIZE_NONE, 
       NULL
      );

      XmStringFree(motif_message);
      XmStringFree(title_string);

      Widget cancel_btn = XmMessageBoxGetChild(dialog, XmDIALOG_CANCEL_BUTTON);
      Widget help_btn = XmMessageBoxGetChild(dialog, XmDIALOG_HELP_BUTTON);
      XtUnmanageChild(cancel_btn);
      XtUnmanageChild(help_btn);

      XtAddCallback(dialog, XmNokCallback, on_close_dialog, (XtPointer)dialog);
      //

      if(!is_numeric(age_field)){
		  printf("Validation Error, please enter a valid number for age! \n");
	  }else {
		  int age = atoi(age_field);
	      
	      if(age < 0 || age > 130){ 
			    printf("Validation Error : Age %d is out or realistic\n", age);
		   } else {
			   printf("Profile Submitted success!\n");
			   printf("Form Submitted: name: %s , city: %s , age: %d\n", name, city, age);
		       // save data
               UserData *new_user = (UserData *)malloc(sizeof(UserData));
               if(new_user != NULL){
                    snprintf(new_user->name, sizeof(new_user->name), "%s", name);
                    snprintf(new_user->city, sizeof(new_user->city), "%s", city);
                    new_user->age = age;

                    new_user->next = *(fields->list_head);
                    *(fields->list_head) = new_user;

                    printf("Success saved into list memory!\n");

                }

                char display_line[256];
                snprintf(display_line, sizeof(display_line), "%s (%d) - %s", name, age, city);

                XmString motif_line = XmStringCreateLocalized(display_line);

                XmListAddItem(fields->display_list, motif_line, 0);
                XmStringFree(motif_line);


               XmTextFieldSetString(fields->name_field, "");
               XmTextFieldSetString(fields->age_field, "");
               XmTextFieldSetString(fields->city_field, "");

               // open dialog on screen
               XtManageChild(dialog);
           
            }	  
	  }
      
      
      
      XtFree(name);	
      XtFree(city);
      XtFree(age_field);
}

void submit_clicked(Widget w, XtPointer client_data, XtPointer call_data){

   Widget text_field = (Widget)client_data;
   
   char *user_input = XmTextFieldGetString(text_field);
   
   printf("Submitted: %s\n", user_input);
   
   XtFree(user_input);	
}

void on_nav_click(Widget w, XtPointer client_data, XtPointer call_data){
     FormFields *app_ctx = (FormFields *)client_data;

     char*button_name = XtName(w);

     if(strcmp(button_name, "navHome") == 0){
        XtUnmanageChild(app_ctx->about_view_panel);
        XtManageChild(app_ctx->home_view_panel);
     }else if(strcmp(button_name, "navAbout") == 0){
        XtUnmanageChild(app_ctx->home_view_panel);
        XtManageChild(app_ctx->about_view_panel);
     }

}

int main(int argc, char *argv[]) { // Added standard arguments required by initialization
    printf("\n");
    
    UserData *user_list_head = NULL;

    XtAppContext app;
    Widget top_level, row_col,main_form, main_content_window, left_sidebar;
    Widget  lbl_right, details_box, center_window, right_sidebar, sidebar;
    Widget button,lbl1, lbl2, lbl3, text_field, submit_btn;
    Widget center_workspace, lbl_about_text, scroll_list_frame;
    Widget btn_home, btn_about, left_sidebar_separator, right_sidebar_separator;
    
    FormFields *my_form = (FormFields *)malloc(sizeof(FormFields));

    my_form->list_head = & user_list_head;
    
    // 1. Fixed "PtAppInitialize" typo to "XtVaAppInitialize"
    // 2. Passed &argc and argv instead of zeros to correctly initialize X Toolkit
    top_level = XtVaAppInitialize(&app, "Hello", NULL, 0, &argc, argv, NULL, 
        XmNwidth, 1000, XmNheight, 700,
        
        NULL);

    //XtVaSetValues(top_level, XmNwidth, 1000, XmNheight, 700, NULL);

    main_form = XmCreateForm(top_level, "mainForm", NULL, 0);
    XtManageChild(main_form);

    

    
    // left_sidebar
    left_sidebar = XmCreateRowColumn(main_form, "leftSidebar", NULL, 0);
    XtVaSetValues(
        left_sidebar,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNwidth, 180,
        NULL
    );
    XtManageChild(left_sidebar);

    // left_sidebar items
    btn_home = XmCreatePushButton(left_sidebar, "navHome", NULL, 0);
    XmString btn_home_lbl = XmStringCreateLocalized("Home Form View");
    XtVaSetValues(btn_home, XmNlabelString, btn_home_lbl, NULL);
    XmStringFree(btn_home_lbl);
    XtManageChild(btn_home);
    XtAddCallback(btn_home, XmNactivateCallback, on_nav_click, (XtPointer)my_form);

    btn_about = XmCreatePushButton(left_sidebar, "navAbout", NULL, 0);
    XmString btn_about_lbl = XmStringCreateLocalized("About View");
    XtVaSetValues(btn_about, XmNlabelString, btn_about_lbl, NULL);
    XmStringFree(btn_about_lbl);
    XtManageChild(btn_about);
    XtAddCallback(btn_about, XmNactivateCallback, on_nav_click, (XtPointer)my_form);

    // right_sidebar
    right_sidebar = XmCreateRowColumn(main_form, "rightSidebar", NULL, 0);
    XtVaSetValues(
        right_sidebar,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNwidth, 180,
        NULL
    );
    XtManageChild(right_sidebar);

    lbl_right = XmCreateLabel(right_sidebar, "Notes", NULL, 0);
    XtManageChild(lbl_right);
    details_box = XmCreateScrolledText(right_sidebar, "detailsBox", NULL, 0);
    XtVaSetValues(details_box, 
        XmNeditMode,
         XmMULTI_LINE_EDIT,
          XmNrows, 10,
          //XmNcolumns, 20,
          NULL);
    XtManageChild(details_box);
    XtManageChild(XtParent(details_box));

    // left_side_separator
    left_sidebar_separator = XtVaCreateManagedWidget("leftSidebarSep", 
        xmSeparatorWidgetClass, main_form, 
        XmNorientation, XmVERTICAL,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNleftWidget, left_sidebar,
        XmNleftOffset, 100,
        XmNwidth, 50,
        NULL);

         // left_side_separator
    right_sidebar_separator = XtVaCreateManagedWidget("rightSidebarSep", 
        xmSeparatorWidgetClass, main_form, 
        XmNorientation, XmVERTICAL,
        XmNseparatorType, XmSHADOW_ETCHED_IN,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        XmNrightWidget, right_sidebar,
        XmNrightOffset, 150,
        XmNwidth, 50,
        NULL);

    // center_workspace
    center_workspace = XmCreateForm(main_form, "mainForm", NULL, 0);
    //XtManageChild(main_form);
    XtVaSetValues(center_workspace,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,

        XmNleftAttachment, XmATTACH_WIDGET,
        XmNleftWidget, left_sidebar_separator,
        XmNleftOffset, 15,


        XmNrightAttachment, XmATTACH_FORM,
        XmNrightWidget, right_sidebar_separator,
        XmNleftOffset, 15,

        //XmNwidth, 400,
        
        NULL
    );
    XtManageChild(center_workspace);

    // home_view_panel

    my_form->home_view_panel = XmCreateRowColumn(center_workspace,"homeView", NULL, 0 );
    XtVaSetValues(my_form->home_view_panel,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        //XmNwidth, 300,
        NULL
    );
    XtManageChild(my_form->home_view_panel);

    
    // text_field
    //text_field = XmCreateTextField(row_col, "inputField", NULL, 0);
    //XtManageChild(text_field);

    // form
    
    lbl1 = XmCreateLabel(my_form->home_view_panel, "Name:", NULL, 0);
    XtManageChild(lbl1);
    my_form->name_field = XmCreateTextField(my_form->home_view_panel, "Name", NULL, 0);
    XtManageChild(my_form->name_field);
    
    lbl2 = XmCreateLabel(my_form->home_view_panel, "Age:", NULL, 0);
    XtManageChild(lbl2);
    my_form->age_field = XmCreateTextField(my_form->home_view_panel, "Age", NULL, 0);
    XtManageChild(my_form->age_field);
    
    
    lbl3 = XmCreateLabel(my_form->home_view_panel, "City:", NULL, 0);
    XtManageChild(lbl3);
    my_form->city_field = XmCreateTextField(my_form->home_view_panel, "City", NULL, 0);
    XtManageChild(my_form->city_field);
    
    
    // 3. Fixed the variable typo 'o' to the digit '0'
    button = XmCreatePushButton(my_form->home_view_panel, "Click", NULL, 0);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, button_clicked, NULL);
    
    submit_btn = XmCreatePushButton(my_form->home_view_panel, "Submit", NULL, 0);
    XtManageChild(submit_btn);
    XtAddCallback(submit_btn, XmNactivateCallback, on_submit, (XPointer)my_form);
    //XtAddCallback(submit_btn, XmNactivateCallback, submit_clicked, (XPointer)text_field);
    

     my_form->display_list = XmCreateScrolledList(my_form->home_view_panel, "userListView", NULL, 0);
    scroll_list_frame = XtParent(my_form->display_list);
    XtVaSetValues(my_form->display_list, XmNvisibleItemCount, 8, NULL);
    XtManageChild(my_form->display_list);
    /*XtVaSetValues(
      scroll_list_frame,
      XmNtopAttachment, XmATTACH_FORM,
      XmNbottomAttachment, XmATTACH_FORM,
      XmNleftAttachment, XmATTACH_WIDGET,
      XmNleftWidget, left_sidebar,
      XmNleftOffset, 10,
      XmNrightAttachment, XmATTACH_WIDGET,
      XmNrightWidget, right_sidebar,
      XmNrightOffset, 10,

      NULL
    );*/

    // about view
    my_form->about_view_panel = XmCreateRowColumn(center_workspace, "aboutView", NULL, 0);
    XtVaSetValues(my_form->about_view_panel,
        XmNtopAttachment, XmATTACH_FORM,
        XmNbottomAttachment, XmATTACH_FORM,
        XmNleftAttachment, XmATTACH_FORM,
        XmNrightAttachment, XmATTACH_FORM,
        NULL
    );
    //XtManageChild(my_form->about_view_panel);

    lbl_about_text = XmCreateLabel(my_form->about_view_panel,
    "--- Motif Navigation System ---\n\n"
        "This software demonstrates layered structural widgets.\n"
        "Built securely using standard components in C.",
    NULL, 0
    );
    XtManageChild(lbl_about_text);
    

    XtRealizeWidget(top_level);
    XtAppMainLoop(app);
    
    free(my_form);
    
    printf("\n\n\n");
    return 0;
}
