
#include "interpret.h"
#include "lexikalny_analyzator.h"
#include "garbage_collector.h"
#include "syntakticky_analyzator.h"
#include "ial.h"
#include "vest_funkce.h"
#include "advanced_strings.h"
#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void find_labels(TA_Item **ta_table, int **jump_array)// hledani navesti kam se ma skocit
{
    *jump_array = advRealloc(*jump_array, ALL_MORE * sizeof(int));
    int jump = 0;
    index_pb = 0;
    int a_size = ALL_MORE;
    if((ta_table) == NULL)
    {
        return;
    }
    int end = ((*ta_table)[0].end);
    for(int position = 0; position <= end; position++)
    {
        if((*ta_table)[position].operation==I_LABEL)
        {
            jump=(*ta_table)[position].jump;
            if(jump >= a_size)
            {
                *jump_array = advRealloc( *jump_array, (ALL_MORE + a_size) * sizeof(int));
                if( *ta_table == NULL)  // chyba alokace
                {
                    error = EINT;
                    return;
                }
                a_size += ALL_MORE;
            }

            (*jump_array)[jump] = position;
        }
    }
}

void destroy_labels(int *jump_array)
{
    //free(jump_array);
    jump_array = NULL;
}

void instrukce(TA_Item **ta_table, tBTSUzolPtr root)
{
    if((*ta_table)==NULL)  //prazdny program, nemame co resit, koncime
    {
        return;
    }
    int end= (*ta_table)[0].end;
    size_t operation;
    tBTSUzolPtr op1, op2, result, pom;
    op1 = NULL;
    op2 = NULL;
    result = NULL;
    pom = NULL;
    int *jump_array=NULL;
    find_labels(ta_table, &jump_array);

    for(int position=0; position < end; position++)
    {
        ta_Get(ta_table, &operation, &op1, &op2, &result, position);
        switch(operation)
        {
            /******************aritmeticke operace**********************/ // OPERACE NA ZAKLADE VYRAZY.C
        case I_ADD:
            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == tDouble) && (op2->data.typ == tDouble))// double + double
            {
                result->data.value.d = (op1->data.value.d) + (op2->data.value.d);
                result->data.typ = tDouble;
            }
            else if((op1->data.typ == tString) && (op2->data.typ == tString))// string.string
            {
                string text1, text2;
                text1.data = op1->data.value.s;
                text2.data = op2->data.value.s;
                result->data.value.s = strAppendString(&text1, &text2);
            }
            else if ((op1->data.typ == tInt) && (op2->data.typ == tInt))//int + int
            {
                result->data.value.i == (op1->data.value.i) + (op2->data.value.i);
                result.data.typ = tInt;
            }
            else if ((op1->data.typ != tString) && (op2->data.typ != tString))
            {
                if ((op1->data.typ != tInt) && (op2->data.typ == tInt)) // double+int
                {
                    op2->data.value.d = (double)op2->data.value.i;
                    result->data.value.d == (op1->data.value.d) + (op2->data.value.d);
                    result.data.typ = tDouble;
                }
                else// int+double
                {
                    op1->data.value.d = (double)op1->data.value.i;
                    result->data.value.d == (op1->data.value.d) + (op2->data.value.d);
                    result.data.typ = tDouble;
                }

            }

            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                return;
            }
            break;

        case I_SUB:// stejne jako u scitani, bez string, protoze string se neda odecitavat
            if(result==NULL)
            {
                break;
            }
            if((op1->data.typ==tDouble) && (op2->data.typ==tDouble))
            {
                result->data.value.d= op1->data.value.d - op2->data.value.d;
                result->data.typ=tDouble;
            }
            else if ((op1->data.typ == tInt) && (op2->data.typ == tInt))
            {
                result->data.value.i = (op1->data.value.i) - (op2->data.value.i);
                result.data.typ = tInt;
            }
            else if ((op1->data.typ != tString) && (op2->data.typ != tString))
            {
                if ((op1->data.typ != tInt) && (op2->data.typ == tInt))
                {
                    op2->data.value.d = (double)op2->data.value.i;
                    result->data.value.d = (op1->data.value.d) - (op2->data.value.d);
                    result.data.typ = tDouble;
                }
                else
                {
                    op1->data.value.d = (double)op1->data.value.i;
                    result->data.value.d = (op1->data.value.d) - (op2->data.value.d);
                    result.data.typ = tDouble;
                }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                return;
            }
            break;

        case I_MULD:// nasobeni
            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == tDouble) && (op2->data.typ == tDouble))
            {
                result->data.value.d = op1->data.value.d * op2->data.value.d;
                result->data.typ = tDouble;
            }
            else if ((op1->data.typ == tInt) && (op2->data.typ == tInt))
            {
                result->data.value.i = (op1->data.value.i) * (op2->data.value.i);
                result.data.typ = tInt;
            }
            else if ((op1->data.typ != tString) && (op2->data.typ != tString))
            {
                if ((op1->data.typ != tInt) && (op2->data.typ == tInt))
                {
                    op2->data.value.d = (double)op2->data.value.i;
                    result->data.value.d == (op1->data.value.d) * (op2->data.value.d);
                    result.data.typ = tDouble;
                }
                else
                {
                    op1->data.value.d = (double)op1->data.value.i;
                    result->data.value.d == (op1->data.value.d) * (op2->data.value.d);
                    result.data.typ = tDouble;
                }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_DIVD://DELENI PRIDAT I_DIV CELE + DESETINNE CISLO VE VYRAZY.C, DELENI DESE. CISLO
            if(result == NULL)
            {
                break;
            }
            if((op1->data.typ == tDouble) && (op2->data.typ == tDouble))
            {
                if(op2->data.value.d == 0.0)
                {
                    error = EINT;
                    ta_Destroy(ta_table);
                    return;
                }
                result->data.value.d= op1->data.value.d / op2->data.value.d;
                result->data.typ=tDouble;
            }
            else if ((op1->data.typ != tString) && (op2->data.typ != tString))
            {
                if ((op1->data.typ != tInt) && (op2->data.typ == tInt))
                {
                    if(op2->data.value.d == 0)
                    {
                        error = EINT;
                        ta_Destroy(ta_table);
                        return;
                    }
                    op2->data.value.d = (double)op2->data.value.i;
                    result->data.value.d = (op1->data.value.d) / (op2->data.value.d);
                    result.data.typ = tDouble;
                }
                else
                {
                    if(op2->data.value.d == 0.0)
                    {
                        error = EINT;
                        ta_Destroy(ta_table);
                        return;
                    }
                    op1->data.value.d = (double)op1->data.value.i;
                    result->data.value.d = (op1->data.value.d) / (op2->data.value.d);
                    result.data.typ = tDouble;
                }

            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                return;
            }
            break;

        case I_DIVD:
            if(result == NULL)
            {
                break;
            }
            else if ((op1->data.typ == tInt) && (op2->data.typ == tInt))
            {
                if(op2->data.value.i == 0)
                {
                    error = EINT;
                    ta_Destroy(ta_table);
                    return;
                }
                result->data.value.i = (op1->data.value.i) / (op2->data.value.i);
                result.data.typ = tInt;
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                return;
            }
            break;

        case I_ASSIGN:
            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == tBool)
            {
                result->data.value.b=op1->data.value.b;
                result->data.typ=tBool;
            }
            else if(op1->data.typ==tDouble)
            {
                result->data.value.d=op1->data.value.d;
                result->data.typ=tDouble;
            }
            else if(op1->data.typ==tString)
            {
                result->data.value.s = op1->data.value.s;
                result->data.typ=tString;
            }
            else if(op1->data.typ==tNil)
            {
                result->data.typ=tNil;
                result->data.value.b=false;
            }
            else if(op1->data.typ==tInt)
            {
                result->data.value.i = op1->data.value.i;
                result->data.typ=tInt;
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                return;
            }
            break;

        case I_POW:// EXPONENCIAL
            if(result==NULL)
            {
                break;
            }
            if((op1->data.typ == tDouble) && (op2->data.typ==tDouble))
            {
                result->data.value.d = pow(op1->data.value.d, op2->data.value.d);
                result->data.typ = tDouble;
            }
            if((op1->data.typ == tInt) && (op2->data.typ==tInt))
            {
                result->data.value.i = pow(op1->data.value.i, op2->data.value.i);
                result->data.typ = tInt;
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;
            /****************porovnani**********************/
        case I_EQUAL:
            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op2->data.value.i == op1->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ=tBool;
                    }
                }
            }
            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op2->data.value.d == op1->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ=tBool;
                    }
                }
            }
            else if(op1->data.typ == tBool)
            {
                if(op2->data.typ != tBool)
                {
                    result->data.value.b=false;
                    result->data.typ=tBool;
                }
                else
                {
                    if(op2->data.value.b == op1->data.value.b)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) == 0)
                {
                    result->data.value.b=true;
                    result->data.typ = tBool;
                }
                else
                {
                    result->data.value.b=false;
                    result->data.typ = tBool;
                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_NEQUAL:
            if(result==NULL)
            {
                break;
            }
            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.i != op2->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }

            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.d != op2->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }

            else if(op1->data.typ == tBool)
            {
                if(op2->data.typ != tBool)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.b!=op2->data.value.b)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) !=0)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_ELESS:
            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.i <= op2->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.d <= op2->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if(op1->data.typ == tBool)
            {
                if(op2->data.typ != tBool)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op2->data.value.b == op1->data.value.b)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) <= 0)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_LESS:
            if(result == NULL)
            {
                break;
            }

            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.i < op2->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }

            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.d < op2->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) < 0)
                {
                    result->data.typ = tBool;
                    result->data.value.b = true;
                }
                else
                {
                    result->data.typ = tBool;
                    result->data.value.b = false;
                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_EMORE:
            if(result == NULL)
            {
                break;
            }

            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.i >= op2->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }

            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.d >= op2->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if(op1->data.typ == tBool)
            {
                if(op2->data.typ != tBool)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op2->data.value.b == op1->data.value.b)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) >= 0)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    result->data.value.b=false;
                    result->data.typ = tBool;
                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_MORE:
            if(result == NULL)
            {
                break;
            }
            if(op1->data.typ == tInt)
            {
                if(op2->data.typ != tInt)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.i > op2->data.value.i)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if(op1->data.typ == tDouble)
            {
                if(op2->data.typ != tDouble)
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }
                else
                {
                    if(op1->data.value.d > op2->data.value.d)
                    {
                        result->data.value.b = true;
                        result->data.typ = tBool;
                    }
                    else
                    {
                        result->data.value.b = false;
                        result->data.typ = tBool;
                    }
                }
            }
            else if (op1->data.typ == tString && op2->data.typ == tString)
            {
                if(strcmp(op1->data.value.s,op2->data.value.s) > 0)
                {
                    result->data.value.b = true;
                    result->data.typ = tBool;
                }
                else
                {
                    result->data.value.b = false;
                    result->data.typ = tBool;
                }

            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

            /**********vestavene fce*************/
        case I_INPUT:
            if(op1 == NULL)
            {
                break;
            }
            if(op1->data.typ == tDouble)
            {
                if(result == NULL)
                {
                    return;
                }
                string text;
				        strCreate(&text);

                int orez = op1->data.value.d;
                int ch_znak = 0;
                while(orez > 0)
                {

                    ch_znak = getchar();
                    if(result != NULL)
                    {

						strAppendChar(&text, (char) ch_znak);

                    }
                  (orez)--;
                }
                result->data.value.s = text.data;
                result->data.typ = tString;

            }
            else if (op1->data.typ == tString)
            {
                int prepinac = -1;

                if(strcmp(op1->data.value.s,"*n") ==0 )
                {
                    prepinac = 1;
                }
                else if(strcmp(op1->data.value.s,"*l")==0)
                {
                    prepinac = 2;
                }
                else if(strcmp(op1->data.value.s,"*a")==0)
                {
                    prepinac = 3;
                }
                else
                {
                    error = ESEM;
                }

                string text;
                strCreate(&text);
                int znak = 0;
                int ret = 0; // pouze overeni scanf

                switch(prepinac)
                {
                case -1:
                    return;
                    break;

                case 1:
                    if(result!=NULL)
                    {
                        ret = scanf("%lf",&(result->data.value.d));
                        if(ret != 1) {
                            error = EINT;
                            return;
                        }
                        result->data.typ=tDouble;
                    }
                    break;

                case 2:
                    while(1)
                    {
                        {

                            char ch_znak = 0;
                            znak=getchar();
                            if((znak==10) || (znak==13))
                            {
                                break;
                            }
                            ch_znak=znak;
                            strAppendChar(&text, ch_znak);


                        }
                    }
                    if(result!=NULL)
                    {
                        result->data.value.s = text.data;
                        result->data.typ=tString;
                    }
                    break;

                case 3:
                    while(1)
                    {
                        {
                            if((znak=getchar()) == EOF)
                            {
                                break;
                            }
                            char ch_znak=0;
                            ch_znak=znak;
                            strAppendChar(&text, ch_znak);
                        }
                    }
                    if(result!=NULL)
                    {
                        result->data.value.s=text.data;
                        result->data.typ=tString;
                    }

                }
            }
            else
            {
                error = ESEM;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_WRITE:
            if(op1 == NULL)
            {
                break;
            }
            if(op1->data.typ == tDouble)
            {
                printf("%lf", op1->data.value.d);
            }
            else if(op1->data.typ == tString)
            {
                char *retezec = op1->data.value.s;
                if(retezec == NULL)
                {
                    break;
                }
                for(int i=0; retezec[i] != '\0' ; i++)
                {
                    if( retezec[i]=='\\' )
                    {
                        if( retezec[i+1]=='n' )
                        {
                            printf("\n");
                            i++;
                        }
                        else if( retezec[i+1]=='t' )
                        {
                            printf("\t");
                            i++;
                        }
                        else if( retezec[i+1]=='\\' )
                        {
                            printf("\\");
                            i++;
                        }
                        else if( retezec[i+1]=='\"' )
                        {
                            printf("\"");
                            i++;
                        }
                        else if( isdigit(retezec[i+1]) && isdigit(retezec[i+2]) && isdigit(retezec[i+3]) )
                        {
                            int ascii= atoi(&retezec[i+1]);
                            putchar(ascii);
                            i=i+3;
                        }
                    }
                    else
                    {
                        putchar(retezec[i]);
                    }
                }

            }
            else
            {
                error = EINT;
                ta_Destroy(ta_table);
                destroy_labels(jump_array);
                return;
            }
            break;

        case I_Asc:
            if(op1==NULL)
            {
                break;
            }
            if(op2==NULL)
            {
                break;
            }
            pom = Asc(op1,op2);
            if(result != NULL)
            {
                if(pom->data.typ == tInt)
                {
                    result->data.value.i = pom; //pom->data.value.i;
                    result->data.typ = tInt;
                }
                else if(pom->data.ty p== tNil)
                {
                    result->data.typ = tNil;
                }
            }
            break;

        case I_SUBS:
            pom = substr(op1, op2, result);
            position++;
            ta_Get(ta_table, &operation, &op1, &op2, &result, position);
            if(result! = NULL)
            {
                if(pom->data.typ == tString)
                {
                    result->data.value.s = pom; //pom->data.value.s;
                    result->data.typ = tString;

                }
                else if(pom->data.typ == tNil)
                {
                    result->data.typ=tNil;

                }
            }
            break;

        case I_Chr:
            if(op1 == NULL)
            {
                break;
            }
            pom = Chr(op1);

            if(pom->data.typ == tString)
            {
                result->data.value.s = pom->data.value.s;
                result->data.typ = tString;


            }
            else if(pom->data.typ == tNil)
            {
                result->data.typ = tNil;


            }
            else
            break;

        case I_Length:
            if(op1 == NULL)
            {
                break;
            }
            pom = Length(op1);
            if(pom->data.typ == tInt)
            {
                result->data.value.i = pom; //pom->data.value.i;
                result->data.typ = tInt;
            }
            break;
            /********************* jumpy a labely ****************************/
        case  I_LABEL:
            break;

        case I_TJUMP:
            if((op1->data.typ == tBool) && (op1->data.value.b == true))
            {
                position = jump_array[(*ta_table)[position].jump];
            }
            else if((op1->data.typ == tDouble) && (op1->data.value.d != 0))
            {
                position = jump_array[(*ta_table)[position].jump];
            }
            break;

        case I_FJUMP:
            if((op1->data.typ == tBool) && (op1->data.value.b == false))
            {
                position = jump_array[(*ta_table)[position].jump];
            }
            else if((op1->data.typ == tDouble) && (op1->data.value.d == 0))
            {
                position = jump_array[(*ta_table)[position].jump];
            }
            break;

        case I_JUMP:
            position = jump_array[(*ta_table)[position].jump];
            break;
        case I_FCEJUMP:
            pole_back[index_pb] = (position);
            index_pb++;
            if((index_pb % 9) == 0) /*naalokovat jinak*/
            {
                pole_back = advRealloc( pole_back, index_pb*sizeof(int)+ALL_MORE);
            }

            position = jump_array[(*ta_table)[position].jump];
            break;
        case I_GOTO:
            index_pb--;
            if(index_pb < 0)
            {
                return;
            }
            else
            {
                position=pole_back[index_pb];

            }
            break;
        }
    }
    destroy_labels(jump_array);
}

