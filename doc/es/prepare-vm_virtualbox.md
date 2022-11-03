# Preparación de la maquina virtual: VirtualBox

## Requisitos

Será necesario:

1. Instalar VirtualBox ([Link a zona de descargas](https://www.virtualbox.org/wiki/Downloads))
2. Descargar la imagen de disco:
    - [vm-devel_disk.vdi.zip](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/zip/vm-devel_disk.vdi.zip)
    - [vm-devel_disk.vdi.z01](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z01/vm-devel_disk.vdi.z01)
    - [vm-devel_disk.vdi.z02](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z02/vm-devel_disk.vdi.z02)
    - [vm-devel_disk.vdi.z03](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z03/vm-devel_disk.vdi.z03)
    - [vm-devel_disk.vdi.z04](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z04/vm-devel_disk.vdi.z04)
    - [vm-devel_disk.vdi.z05](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z05/vm-devel_disk.vdi.z05)
    - [vm-devel_disk.vdi.z06](https://laboratorio-ris.gitlab.io/vm-devel_disk/vdi/z06/vm-devel_disk.vdi.z06)
3. Descomprimir/extraer la imagen, obteniendo un único fichero grande.
    > **NOTA**: Antes de borrar las partes, confirmar el correcto funcionamiento de la imagen

## Creación de la máquina virtual

Los datos de la máquina a crear son los siguientes:

- **Tipo**: Linux
- **Versión**: Ubuntu 64 bits
- **Tamaño de memoria**: 2048 Mb
- **Disco duro**: El anteriormente descargado y descomprimido
- **Chipset**: ICH9
- **Procesador**: 2
- **Paravirtualización**: KVM (+ Nested paging)
- **Driver disco duro**: AHCI (SATA)
- **Driver VGA**: VMSVGA

A continuación se proporciona una serie de capturas donde puede verse el proceso de configuración, paso a paso y en orden, y como debe aparecer el entorno una vez todo funcione correctamente:

### 1. Instalación: Pantalla principal de VirtualBox

Al abrir VirtualBox, si no tenemos previamente otras máquinas, se deberia ver una ventana similar a la de la siguiente figura, donde se deberá crear una nueva máquina mediante el botón `New`,

![Instalación: Pantalla principal de VirtualBox](../img/vbox_main-screen.png)

### 2. Instalación: Máquina sin almacenamiento

Se introducirán los datos requeridos:

- **Name**: Nombre que se le dará a la máquina. Útil si hay mas de una, indiferente con pocas o una.
- **Machine folder**: Carpeta donde se guardará la información de la máquina. Se recomienda que la imagen descargada y descomprimida se ponga en esta carpeta.
- **Type**: Como usaremos 'LUbuntu', una versión reducida de 'Ubuntu', se selecciona `Linux`.
- **Version**: Por el mismo motivo se selecciona `Ubuntu (64-bit)`.
- **Memory size**: Se deberan asignar al menos 2 Gb (2048 Mb), ya que con menos se observan errores.
- **Hard disk**: Se selecciona el uso de un disco ya existente y se presiona el icono de la carpeta.

![Instalación: Máquina sin almacenamiento](../img/vbox_new-machine_no-disk.png)

### 3. Instalación: Gestor de almacenamiento

Esta ventana permitirá la creación de discos duros desde el punto de vista de VirtualBox, lo que permitirá a las máquinas virtuales poder usarlo.

Para poder buscar y asignar la imagen de disco descargada, se presiona el boton de 'Add'.

![Instalación: Gestor de almacenamiento](../img/vbox_new-machine_new-disk_empty.png)

### 4. Instalación: Selección de fichero de imagen de disco

Como muestra la figura, la imagen descargada (tras descomprimirla) debería poder seleccionarse sin problema.

![Instalación: Selección de fichero de imagen de disco](../img/vbox_new-machine_new-disk_open.png)

### 5. Instalación: Imagen seleccionada

Una vez VirtualBox valida la imagen, puede verse como ya aparece disponible para su uso, indicando de paso el tamaño real interno.

Se selecciona y se pulsa 'Choose' para asociar el disco con la máquina en creación.

![Instalación: Imagen seleccionada](../img/vbox_new-machine_new-disk_ok.png)

### 6. Instalación: Maquina con almacenamiento

Si no surge ningún problema, la máquina debería mostrar algo similar a la siguiente figura y se podrá pasar a pulsar 'Create'.

![Instalación: Maquina con almacenamiento](../img/vbox_new-machine_with-disk.png)

Una vez esté creada aparecerá en la lista de la pantalla principal. Ahora se procede a darle los últimos retoques mediante el botón de 'Settings'.

### 7. Configuración: General > Básica

La pantalla de inicio muestra la información básica: nombre y sistema operativo configurados. No es necesario modificar nada.

![Configuración: General > Básica](../img/vbox_vm-devel_details-general-basic.png)

### 8. Configuración: General > Avanzada

En el caso del ejemplo no se pretende ni compartir portapapeles, ni utilizaar drag-n-drop, como muestra la figura:

![Configuración: General > Avanzada](../img/vbox_vm-devel_details-general-advanced.png)

### 9. Configuración: Sistema > Placa base

En la configuración del sistema deberá asegurarse una configuración como la mostrada en la figura. Prestar especial atención en modificar el chipset a `ICH9`.

![Configuración: Sistema > Placa base](../img/vbox_vm-devel_details-system-motherboard.png)

### 10. Configuración: Sistema > Procesador

En este apartado es necesario asegurar una asignación de, al menos, 2 CPU.

![Configuración: Sistema > Procesador](../img/vbox_vm-devel_details-system-processor.png)

### 11. Configuración: Sistema > Aceleración

Asegurar la selección de KVM y paginado anidado, como muestra la figura:

![Configuración: Sistema > Aceleración](../img/vbox_vm-devel_details-system-acceleration.png)

### 12. Configuración: Red > Adaptador 1 con NAT

Por último, asegurar el tipo de red (que esté habilitada y que esté configurada como NAT)

![Configuración: Red > Adaptador 1 con NAT](../img/vbox_vm-devel_details-network-adapter-1.png)

Una vez se termine este paso, ya se puede pulsar 'OK' y se puede arrancar la máquina.

### 13. Maquina virtual: Arrancando

Si todo ha ido como se espera es posible que aparezcan algunos warnings por pantalla, pero acabará mostrandose algo como:

![Maquina virtual: Arrancando](../img/vbox_vm-devel_starting.png)

### 14. Maquina virtual: Primer arranque

Se da por arrancada la máquina cuando se puede ver el el escritorio, pero se recomienda mejorar las dimensiones del monitor, como se ve en el siguiente apartado.

![Maquina virtual: Primer arranque](../img/vbox_vm-devel_first-run.png)

### 15. Maquina virtual: Configuración monitor

La siguiente figura muestra donde puede localizarse el gestor de monitores.

![Maquina virtual: Configuración monitor](../img/vbox_vm-devel_configure-monitor_menu.png)

### 16. Maquina virtual: Configurado 1440x900

Ejemplo mostrando la nueva configuracion de 1440x900 (resolución tipica en portatiles).

![Maquina virtual: Configurado 1440x900](../img/vbox_vm-devel_configure-monitor_saved.png)

## Siguientes pasos

Alcanzado este punto, la máquina virtual se considera creada, configurada y arrancada.

En este momento es fiable proceder al siguiente apartado.

## Recomendaciones

### Actualizar la máquina virtual

Una vez instalada y correctamente configurada, se recomienda actualizar la máquina virtual para disponer de los últimas versiones de los programas.

```bash
    sudo apt -y update
    sudo apt -y upgrade
    sudo apt -y dist-upgrade
    sudo apt -y autoremove
```

### Instalación de las utilidades de VirtualBox
Para poder hacer uso de la compartición de directorios y del portapapeles, se recomienda instalar las utilidades VBoxGuestAdditions en la máquina virtual creada. Para ello, se deben seguir los siguientes pasos:

1. Ir al menú Dispositivos -> Unidades Ópticas y seleccionar la imagen "VBoxGuestAdditions.iso"

![Maquina virtual: Imagen VBoxGuestAdditions.iso](../img/vbox_vm-devel_install_vboxguestadditions.png)

2. Abrir un terminal

3. Ir al directorio del disco

```bash
cd /media/vmu/VBox_GAs_7.0.0
```

4. Ejecutar el instalador como root
```bash
sudo ./VBoxLinuxAdditions.run
```

5. Reiniciar la máquina virtual
```bash
sudo reboot
```

Tras este paso, se podrán habilitar las carpetas compartidas así como ajustar la resolución del sistema operativo virtualizado al tamaño de la ventana.

NOTA: si falla el escritorio, este se puede restaurar ejecutando el comando
```bash
sudo systemctl restart sddm
```

### Instalación de Visual Studio Code

Para proceder a la instalación de Visual Studio Code, se puede realizar de dos formas

#### Instalación mediante paquete .deb

1. Descargar de la web https://code.visualstudio.com/Download el último paquete .deb disponible
2. Ir al directorio de descargas
```bash
cd $HOME/Downloads
3. Instalar dicho paquete
```

```bash
dpkg -i ./<file>.deb
```
#### Instalación desde el repositorio oficial

Este proceso requiere configurar el repositorio oficial de Microsoft. Para ello, se deben ejecutar los siguientes comandos en un terminal:

```bash
    cd $HOME/Documents
    sudo apt-get install wget gpg
    wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
    sudo install -D -o root -g root -m 644 packages.microsoft.gpg /etc/apt/keyrings/packages.microsoft.gpg
    sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/keyrings/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
    rm -f packages.microsoft.gpg
```

Tras instalar el repositorio oficial, se puede proceder a instalar Visual Studio Code a través del gestor apt.
```bash
sudo apt -y install apt-transport-https
sudo apt -y update
sudo apt -y install code
```
