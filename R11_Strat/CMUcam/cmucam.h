
#define LIM_GAUCHE 88
#define LIM_DROITE 264
#define LIM_MARGE 10

#define CMUCAM_FIN 0x0a
#define NB_DATA_IN 25
#define NB_DATA_OUT 25

// *********

enum booleen {
    NON=0,
    OUI
};

enum etat_cmucam_t {
    INIT=0,
    INIT_CASE,
    RECUP_ID_1,
    TEST_ID,
    RECUP_ID_0,
    RE_RECUP_ID_1,
    RE_RECUP_ID_2,
    ENVOI_ID,
    TRACKING,
    TRACKING_PROCHE,
    CMUCAM_RESET,
    PERDU,
    EN_FACE,
    RECUP_ID_ALL,
    RECUP_ID_ALL_SEND,
    RECUP_ID_ALL_RETRY
};

typedef struct  {
  unsigned int x0,x1,y0,y1;
  unsigned char id;
} figure_t;


/** P U B L I C  P R O T O T Y P E S ***************************************/
void CMUcam_Init(void);
char TX_libre(void);
char env_cmucam(void);
char chaine_to_id(char*);
char cherche_pion(void);
char cherche_case_rouge(void);
char cherche_case_bleue(void);
char nouvelle_recherche(void);
char ask_figure(void);
char select_figure(unsigned char);
char chaine_to_figure(char *chaine,figure_t *figure);
char rec_cmucam(char *chaine);
char rec_cmucam_cours(void);
char set_tampon_env(char *chaine);
void CMUcam_int(void);
char cmucam_reset(void);
char get_erreur_RC(void);
void setCouleur(char _c);
char cherche_couleur(void);




