
#include "task_programmer01.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>


// Customized variables are initialized as copy of predefined values
// New programs can be added, date/time can be changed as well as target temperature
// this changes are updated on customized variables, to enable "reset" to predefined values at any time.

#define TV_CUST_ELEMENTS 10
struct target_var     tv_cust[TV_CUST_ELEMENTS];           // initialized as copy of const values. May be personalized.

#define PD_CUST_ELEMENTS 30
struct pattern_daily  pd_cust[PD_CUST_ELEMENTS];           // initialized as copy of const values. May be personalized.

#define PW_CUST_ELEMENTS 50
struct pattern_weekly pw_cust[PW_CUST_ELEMENTS];           // initialized as copy of const values. May be personalized.

#define PGM_TEMP_ELEMENTS 50
struct pattern_pgm active_pattern[PGM_TEMP_ELEMENTS];      // initialized based on previous structures 

static const char *TAG = "TASK_PROGRAMMER01";


#define TV_TEMP_ELEMENTS 5
struct target_var tv_temp[TV_TEMP_ELEMENTS] = {
  {1, 5},     // ºC, antihielo
  {2, 10},    // ºC, temp. baja
  {3, 18},    // ºC, temp. mantenimiento
  {4, 21},    // ºC, temp. comfort
  {5, 23}    // ºC, temp. alta
  };

#define PD_TEMP_ELEMENTS 18 
struct pattern_daily pd_temp[PD_TEMP_ELEMENTS] = {
  {0,1,0,0,1},    // Temperatura Continua, Antihielo
  {1,1,0,0,2},    // Temperatura Continua, Baja
  {2,1,0,0,3},    // Temperatura Continua, Mantenimiento
  {3,1,0,0,4},    // Temperatura Continua, Confort
  {4,1,0,0,3},    // Oficina, Confort L-V
  {4,2,6,45,4},   // Oficina, Confort L-V
  {4,3,18,0,3},   // Oficina, Confort L-V
  {5,1,0,0,3},    // Casa, Horario trabajador L-V
  {5,2,5,0,4},    // Casa, Horario trabajador L-V
  {5,3,7,30,3},   // Casa, Horario trabajador L-V
  {5,4,17,0,4},   // Casa, Horario trabajador L-V
  {5,5,22,0,3},   // Casa, Horario trabajador L-V
  {6,1,0,0,3},    // Casa, Ocupación permanente
  {6,2,8,0,4},    // Casa, Ocupación permanente
  {6,3,23,0,3},   // Casa, Ocupación permanente
  {7,1,0,0,2},    // Casa finde, templar viernes tarde
  {7,2,16,0,4},   // Casa finde, templar viernes tarde
  {7,3,23,0,2},   // Casa finde, templar viernes tarde
  };

#define PW_TEMP_ELEMENTS 28
//int pw_temp_elements = 28; 
struct pattern_weekly pw_temp[PW_TEMP_ELEMENTS] = {
  {1,1,4},    // Oficina: confort L-V, Mmto S-D
  {1,2,4},    // Oficina: confort L-V, Mmto S-D
  {1,3,4},    // Oficina: confort L-V, Mmto S-D
  {1,4,4},    // Oficina: confort L-V, Mmto S-D
  {1,5,4},    // Oficina: confort L-V, Mmto S-D
  {1,6,2},    // Oficina: confort L-V, Mmto S-D
  {1,7,2},    // Oficina: confort L-V, Mmto S-D
  {2,1,5},    // Casa: trabajo L-V, confort S-D
  {2,2,5},    // Casa: trabajo L-V, confort S-D
  {2,3,5},    // Casa: trabajo L-V, confort S-D
  {2,4,5},    // Casa: trabajo L-V, confort S-D
  {2,5,7},    // Casa: trabajo L-V, confort S-D
  {2,6,6},    // Casa: trabajo L-V, confort S-D
  {2,7,6},    // Casa: trabajo L-V, confort S-D
  {3,1,6},    // Casa: Ocupación L-D
  {3,2,6},    // Casa: Ocupación L-D
  {3,3,6},    // Casa: Ocupación L-D
  {3,4,6},    // Casa: Ocupación L-D
  {3,5,6},    // Casa: Ocupación L-D
  {3,6,6},    // Casa: Ocupación L-D
  {3,7,6},    // Casa: Ocupación L-D
  {4,1,1},    // Casa de Finde: Calentar el viernes tarde
  {4,2,1},    // Casa de Finde: Calentar el viernes tarde
  {4,3,1},    // Casa de Finde: Calentar el viernes tarde
  {4,4,1},    // Casa de Finde: Calentar el viernes tarde
  {4,5,7},    // Casa de Finde: Calentar el viernes tarde
  {4,6,6},    // Casa de Finde: Calentar el viernes tarde
  {4,7,6}    // Casa de Finde: Calentar el viernes tarde
  };


// internal functions
int lftv(struct target_var *tv);
int lfwp(struct pattern_weekly *pw);
int lfdp(struct pattern_daily *pd);



int tp_init_structures(){

    // init target variable customized
    for (int i=0; i < TV_CUST_ELEMENTS; i++){
        if (i < TV_TEMP_ELEMENTS) {
            tv_cust[i].target_var_ID = tv_temp[i].target_var_ID;
            tv_cust[i].target_var_value = tv_temp[i].target_var_value;
            } else {
            tv_cust[i].target_var_ID = (int) NULL;
            tv_cust[i].target_var_value = (int) NULL;            
            }
        }

    // init patter daily customized
    for (int i=0; i<PD_CUST_ELEMENTS; i++){
        if (i < PD_TEMP_ELEMENTS) {
            pd_cust[i].PD_ID = pd_temp[i].PD_ID;
            pd_cust[i].PD_ID2 = pd_temp[i].PD_ID2;
            pd_cust[i].hour = pd_temp[i].hour;
            pd_cust[i].minute = pd_temp[i].minute;
            pd_cust[i].target_var_ID = pd_temp[i].target_var_ID;        
            } else {
            pd_cust[i].PD_ID =  (int) NULL;
            pd_cust[i].PD_ID2 =  (int) NULL;
            pd_cust[i].hour =  (int) NULL;
            pd_cust[i].minute =  (int) NULL;
            pd_cust[i].target_var_ID =  (int) NULL;
            }
        }

    // init patter weekly customized
    for (int i=0; i<PW_CUST_ELEMENTS; i++){
        if (i < PW_TEMP_ELEMENTS) {
            pw_cust[i].PW_ID = pw_temp[i].PW_ID;
            pw_cust[i].day = pw_temp[i].day;
            pw_cust[i].PD_ID = pw_temp[i].PD_ID;        
            } else {
            pw_cust[i].PW_ID = (int) NULL;
            pw_cust[i].day = (int) NULL;
            pw_cust[i].PD_ID = (int) NULL;        
            }
        }

    return(0);
}


int tp_activate_pattern(int weekly_pattern){

    struct pattern_weekly pw_record;
    struct pattern_daily pd_record;
    struct target_var tv_record;

    int program_records = 0;
    int p_index = 0;
    int error = 0;
    bool pattern_weekly_found = 1;
    bool pattern_daily_found  = 1;

        pw_record.PW_ID = weekly_pattern;       // esto debería ser variable, pero lo pongo como horario de currito
        pw_record.day = 1;                      // El segundo indice empieza siempre en 1   
        do {
            //Foreach weekday, look for records
            error = lfwp(&pw_record);
            if (error == 0){            
                // for each daytime setting, add record
                pattern_weekly_found = 1;
                pd_record.PD_ID = pw_record.PD_ID;
                pd_record.PD_ID2 = 1;           // El segundo indice empieza siempre en 1                
                do {
                    error = lfdp(&pd_record);
                    if (error == 0){
                        pattern_daily_found = 1;
                        tv_record.target_var_ID = pd_record.target_var_ID;
                        error = lftv(&tv_record);
                        if ((error == 0) & (program_records < PGM_TEMP_ELEMENTS)){
                            active_pattern[p_index].day = pw_record.day;
                            active_pattern[p_index].hour = pd_record.hour;
                            active_pattern[p_index].minute = pd_record.minute;
                            active_pattern[p_index].target_var_value = tv_record.target_var_value;
                            p_index++;
                            ESP_LOGI(TAG, "active pattern = {%d, %d, %d, %d}", pw_record.day, pd_record.hour, pd_record.minute, tv_record.target_var_value);
                        }
                        else if (program_records >= PGM_TEMP_ELEMENTS){
                            ESP_LOGE(TAG, "ACTIVE_PATTERN has more than %d records.", PGM_TEMP_ELEMENTS);
                        }
                    } else pattern_daily_found = 0;
                    pd_record.PD_ID2++;
                } while (pattern_daily_found);
            } else pattern_weekly_found = 0;
            pw_record.day++;
        } while (pattern_weekly_found);
        return(0);
}




/* VERIFICO ESTA BUSQUEDA
pd_record.PD_ID = 5;
pd_record.PD_ID2 = 1;
pd_record.hour = 0;
pd_record.minute = 0;
pd_record.target_var_ID = 1;
for (int j=1; j<10; j++){
    ESP_LOGI(TAG, "Buscamos pw_record = {%d, %d, -}", pd_record.PD_ID, pd_record.PD_ID2);
    error = lfdp(&pd_record);
    if (error == 0) {
        ESP_LOGI(TAG, "Encontrado pw_record = {%d, %d, %d, %d, %d}", pd_record.PD_ID, pd_record.PD_ID2, pd_record.hour, pd_record.minute, pd_record.target_var_ID);
    } else ESP_LOGI(TAG, "REGISTRO NO ENCONTRADO");
    //apunto al siguiente
    pd_record.PD_ID2++;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
return(0);
*/

/* VERIFICO ESTA BUSQUEDA
pw_record.PW_ID = weekly_pattern;
pw_record.day = 1;
for (int j=1; j<10; j++){
    ESP_LOGI(TAG, "Buscamos pw_record = {%d, %d, -}", pw_record.PW_ID, pw_record.day);
    error = lfwp(&pw_record);
    ESP_LOGI(TAG, "Encontrado pw_record = {%d, %d, %d}", pw_record.PW_ID, pw_record.day, pw_record.PD_ID);
    //apunto al siguiente
    pw_record.day++;
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
return(0);
*/

/* VERIFICADO, RECUPERA BIEN LOS REGISTROS
for (int j=1; j<6; j++){
    tv_record.target_var_ID = j;
    //ESP_LOGI(TAG, "Buscamos tv_record.target_var_ID = %d", tv_record.target_var_ID);
    error = lftv(&tv_record);
    //ESP_LOGI(TAG, "Encontrado tv_record.target_var_value = %d", tv_record.target_var_value);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
return(0);
*/



/****************************************************************************** 
* lfwp - look for weekly pattern
*******************************************************************************/
// look for weekly pattern in struct list. Supuesto: es una lista ordenada, corregir para caso general 
// @param[in]  *tv->target_var_ID
// @param[out] *tv->target_var_value = value or NULL
int lfwp(struct pattern_weekly *pw){
    int pw_idx = -1;
    //int pw_idx2 = -1;
    int salir = 0;
    int error = 0;
    while (salir == 0) {
        pw_idx++;
        ESP_LOGV(TAG, "lfwp 0 = {%d, %d}", pw->PW_ID, pw->day);
        if (pw_cust[pw_idx].PW_ID == pw->PW_ID) {
            if (pw_cust[pw_idx].day == pw->day) {
                pw->PD_ID = pw_cust[pw_idx].PD_ID;
                salir = 1;
                error = 0;
                ESP_LOGV(TAG, "lfwp 1 = {%d, %d, %d}", pw->PW_ID, pw->day, pw->PD_ID);
                }
            else if (pw_idx == PW_TEMP_ELEMENTS) {
                pw->day =  (int) NULL;
                pw->PD_ID = (int) NULL;            
                salir = 1;
                error = 1;
                ESP_LOGV(TAG, "lfwp 2 = {%d, %d, %d}", pw->PW_ID, pw->day, pw->PD_ID);
                }
            }
        else if (pw_idx == PW_TEMP_ELEMENTS) {
            pw->day =  (int) NULL;
            pw->PD_ID = (int) NULL;            
            salir = 1;
            error = 1;
            ESP_LOGV(TAG, "lfwp 3 = {%d, %d, %d}", pw->PW_ID, pw->day, pw->PD_ID);
            }
    }
    return(error);
}



/****************************************************************************** 
* lfdp - look for dayly pattern
*******************************************************************************/
// look for dayly pattern in struct list. Supuesto: es una lista ordenada, corregir para caso general 
// @param[in]  *tv->target_var_ID
// @param[out] *tv->target_var_value = value or NULL
int lfdp(struct pattern_daily *pd){
    int pd_idx = -1;
    int salir = 0;
    int error = 0;
    while (salir == 0) {
        pd_idx++;
        ESP_LOGV(TAG, "lfdp 0 = {%d, %d}", pd->PD_ID, pd->PD_ID2);
        if (pd_cust[pd_idx].PD_ID == pd->PD_ID) {
            if (pd_cust[pd_idx].PD_ID2 == pd->PD_ID2) {
                pd->hour = pd_cust[pd_idx].hour;
                pd->minute = pd_cust[pd_idx].minute;
                pd->target_var_ID = pd_cust[pd_idx].target_var_ID;          
                salir = 1;
                error = 0;
                ESP_LOGV(TAG, "lfdp 1 = {%d, %d, %d, %d, %d}", pd->PD_ID, pd->PD_ID2, pd->hour, pd->minute, pd->target_var_ID);
                }
            else if (pd_idx == PD_TEMP_ELEMENTS) {
                pd->hour =  (int) NULL;
                pd->minute =  (int) NULL;
                pd->target_var_ID =  (int) NULL;                     
                salir = 1;
                error = 1;
                ESP_LOGV(TAG, "lfdp 2 = {%d, %d, %d, %d, %d}", pd->PD_ID, pd->PD_ID2, pd->hour, pd->minute, pd->target_var_ID);
                }
            }
        else if (pd_idx == PD_TEMP_ELEMENTS) {
            pd->hour =  (int) NULL;
            pd->minute =  (int) NULL;
            pd->target_var_ID =  (int) NULL;                     
            salir = 1;
            error = 1;
            ESP_LOGV(TAG, "lfdp 3 = {%d, %d, %d, %d, %d}", pd->PD_ID, pd->PD_ID2, pd->hour, pd->minute, pd->target_var_ID);
            }
    }
    return(error);
}





/****************************************************************************** 
* lftv - look for target value
*******************************************************************************/
// look for target variable in struct list. Supuesto: es una lista ordenada, corregir para caso general 
// @param[in]  *tv->target_var_ID
// @param[out] *tv->target_var_value = value or NULL
int lftv(struct target_var *tv){
    int tv_idx = -1;
    int salir = 0;
    int error = 0;
    while (salir == 0) {
        tv_idx++;
        if (tv_cust[tv_idx].target_var_ID == tv->target_var_ID) {
            tv->target_var_value = tv_cust[tv_idx].target_var_value;
            salir = 1;
            error = 0;
            }
        else if (tv_idx == TV_TEMP_ELEMENTS) {
            tv->target_var_value = (int) NULL;             
            salir = 1;
            error = 1;
            }
    }
    return(error);
}


/****************************************************************************** 
* get_target_value
*******************************************************************************
 * @brief checks programme and gets target value. 
 * @param[in] actual_time 
 * @param[in] weekly_program_id: active program in use,Index to pw_temp (pattern_weekly.PW_ID).
 * @param[in] override_value: manually changed value, replaces target value until next transition.
 * @param[out] target_value: target value found. 
*******************************************************************************/
/*
int get_target_value(time_t actual_time, int weekly_pgm_id, int override_value, int target_value){

    struct tm timeinfo;
    localtime_r(&actual_time, &timeinfo);

    return(0);
};
*/





