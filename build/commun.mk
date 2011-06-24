
## À modifier si le fichier est utilisé sur un autre pc
## Fichier pour un pic 18f2550
CC	:=	wine "/home/samuel/Robotique/Programmation/Pic/MCC18/bin/mcc18.exe"
LD	:=	wine "/home/samuel/Robotique/Programmation/Pic/Mplab/MPASM Suite/MPLink.exe"

## Indique notamment les header de C18, ainsi que ceux du projets => -I".."
## C'est ici aussi qu'est indiqué le microcontrôleur
CFLAGS  := -p=18F2550 -I"/home/samuel/Robotique/Programmation/Pic/MCC18/h" -I".." -w=2

## Note : j'utilise un bootloader, du coup je n'utilise pas un fichier .lkr fourni avec C18
LFLAGS	:=	 /l"/home/samuel/Robotique/Programmation/Pic/MCC18/lib" /k"/home/samuel/Robotique/Programmation/Pic/lkr" "rm18f2550.lkr" 
## Fin des information concernant le compilateur

RM	:=	rm -f


            
OBJS	:=	$(SRCS:.c=.o)


all	:	$(BIN).hex

flash: all
		sudo fsusb $(BIN).hex


%.hex	:	$(OBJS)
		$(LD) $(LFLAGS) $(OBJS) /z__MPLAB_BUILD=1 /m"$(BIN).map" /w /o"$(BIN).cof"

clean	:
		$(RM) $(OBJS) $(BIN).map $(BIN).cof $(BIN).hex *.err
fclean	:	clean
		$(RM) $(BIN)
re	:	fclean all
%.o	:	%.c
		$(CC) $(CFLAGS) -fo=$@ $< -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-
