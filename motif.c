#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/TextF.h>
#include <Xm/Label.h>
#include <ctype.h>
#include <Xm/MessageB.h>

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
      UserData **list_head;
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

               XmTextFieldSetString(fields->name_field, "");
               
               
               
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


int main(int argc, char *argv[]) { // Added standard arguments required by initialization
    printf("\n");
    
    UserData *user_list_head = NULL;

    XtAppContext app;
    Widget top_level, row_col, button,lbl1, lbl2, lbl3, text_field, submit_btn;
    
    FormFields *my_form = (FormFields *)malloc(sizeof(FormFields));

    my_form->list_head = & user_list_head;
    
    // 1. Fixed "PtAppInitialize" typo to "XtVaAppInitialize"
    // 2. Passed &argc and argv instead of zeros to correctly initialize X Toolkit
    top_level = XtVaAppInitialize(&app, "Hello", NULL, 0, &argc, argv, NULL, 
        XmNwidth, 1000, XmNheight, 700,
        
        NULL);

    //XtVaSetValues(top_level, XmNwidth, 1000, XmNheight, 700, NULL);
    
    row_col = XmCreateRowColumn(top_level, "mainLayout", NULL, 0);
    XtManageChild(row_col);
    
    // text_field
    //text_field = XmCreateTextField(row_col, "inputField", NULL, 0);
    //XtManageChild(text_field);
    
    
    lbl1 = XmCreateLabel(row_col, "Name:", NULL, 0);
    XtManageChild(lbl1);
    my_form->name_field = XmCreateTextField(row_col, "Name", NULL, 0);
    XtManageChild(my_form->name_field);
    
    lbl2 = XmCreateLabel(row_col, "Age:", NULL, 0);
    XtManageChild(lbl2);
    my_form->age_field = XmCreateTextField(row_col, "Age", NULL, 0);
    XtManageChild(my_form->age_field);
    
    
    lbl3 = XmCreateLabel(row_col, "City:", NULL, 0);
    XtManageChild(lbl3);
    my_form->city_field = XmCreateTextField(row_col, "City", NULL, 0);
    XtManageChild(my_form->city_field);
    
    
    
    
    
    // 3. Fixed the variable typo 'o' to the digit '0'
    button = XmCreatePushButton(row_col, "Click", NULL, 0);
    XtManageChild(button);
    XtAddCallback(button, XmNactivateCallback, button_clicked, NULL);
    
    submit_btn = XmCreatePushButton(row_col, "Submit", NULL, 0);
    XtManageChild(submit_btn);
    XtAddCallback(submit_btn, XmNactivateCallback, on_submit, (XPointer)my_form);
    //XtAddCallback(submit_btn, XmNactivateCallback, submit_clicked, (XPointer)text_field);
    
    XtRealizeWidget(top_level);
    XtAppMainLoop(app);
    
    free(my_form);
    
    printf("\n\n\n");
    return 0;
}
