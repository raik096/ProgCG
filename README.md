# ProgCG
Il progetto di computer grafica

///CAMBIAMENTI

POSTUMO:
riga 46 del carousel_loader: cercare di rendere buffer safe la funzione memcpy


->In main.cpp riga 61 e riga 67:
    spostamento con virtual trackball al centro con il premere della rotella invece che con il tasto sx del mouse quindi button = GLFW_MOUSE_BUTTON_MIDDLE invece di         GLFW_MOUSE_BUTTON_LEFT.
    tutto finalizzato a usare il button_left per spostare la scena ed ingrandire quanto voglio

->In main.cpp riga 66 e 70:
    ho cambiato il nome ai meotodi in modo da renderli specifici per la rotellina. Da mouse_press a mouse_middle_press. Stessa cosa nel file di trackball.h riga 124 e     117


->In main.cpp alla riga 255:
    ho cambiato da glDrawArray a glDrawElements, per sfruttare gli indici calcolati nella riga 81 del file carousel_to_renderable.h

->Idea per migliorare il mapping della texture sulla pista:
    rendere proporzionale al deltaX della curva, il fattore di ripetizione. L'idea e' che piu' il delta aumenta quindi piu' distanza copre il triangolo, piu' ripetezioni sono necessarie, al contrario il contrapposto. Inoltre questo fattore deve essere una frazione di un valore condiviso da tutte le ripetizioni in modo che sia tutto distribuito. 
