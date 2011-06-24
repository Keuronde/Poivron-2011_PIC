
extern volatile char data_in[];

void init_i2c(void);
char get_erreur_i2c(void);
char get_etat_i2c(void);
void gestion_i2c(void); // A appeler en interruption
char transmission_i2c(char _adresse, char _nb_env, char _nb_rec, unsigned char *s_envoi);
char i2c_en_cours(void);
void get_i2c_data(unsigned char *str);
extern volatile char data_index;

enum i2c_m_flag {
    I2C_START=0,
    I2C_ENV_ADRESSE,
    I2C_ENV_DONNEE,
    I2C_RESTART,
    I2C_RESTART_2,
    I2C_REC_ADRESSE,
    I2C_REC_DONNEE,
    I2C_REC_ACK,
    I2C_STOP,
    I2C_LIBRE
};
