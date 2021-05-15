# EDIII
Repositorio con los archivos del workspace EDIII 2020 desarrollado sobre MCUXpressoIDE_11.1.1_3241.

La carpeta workspace_unsam contiene los ejemplos realiazdos la primera parte de la cursada, estos se desarrollaron con las bibliotecas de lpcopen y freertos.

La carpeta workspace FreeRTOS+Filter contiene los ejemplos de la segunda parte de la cursada. Aplicamos el procesamiento de señales con la biblioteca cmsis.  

La carpeta Proyecto EDIII+FreeRTOS contiene el proyecto desarrollado para la aprobacio de la cursada. Se encuentran ambas versiones con (y sin) freertos.

## Creacion del repositorio en github

Para realizar el repositorio primero se comenzò realizando un repositorio local, luego se creo un repositorio vacio en github y se subiò los archivos mediante los comandos:

git remote add origin https://github.com/luigi4767/EDIII.git

git branch -M main

git push -u origin main
