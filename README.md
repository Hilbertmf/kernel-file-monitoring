# Primeiros Passos — Projeto de Monitoramento de Kernel

# Replicando o Ambiente — Passos Completos

## 1. Criar a VM no virt-manager
- Hypervisor: QEMU/KVM local
- ISO: debian-11-amd64-netinst.iso
- RAM: 8GB
- vCPUs: 12
- Disco: 80GB
- Firmware: BIOS (não UEFI)
- Marcar "Customize configuration before install"

## 2. Instalar o Debian 11
- Graphical Install
- Hostname: kernellab (ou o que preferir)
- Domain name: deixar em branco
- Criar usuário root e usuário comum
- Particionamento: Guided - use entire disk → All files in one partition
- Mirror: deb.debian.org (Brazil)
- Software selection: Debian desktop environment + GNOME + standard system utilities
- Instalar GRUB no disco principal (/dev/vda)

## 3. Pós-instalação
- Adicionar usuário ao sudo:
  - `su -`
  - `usermod -aG sudo <usuario>`
  - Fazer logout e login novamente

## 4. Snapshot A — VM recém-instalada
- Tirar snapshot antes de qualquer outra modificação

## 5. Instalar spice-vdagent (área de transferência compartilhada) e outras coisas
`sudo apt install spice-vdagent`
- Reiniciar a VM
- Confirmar que copy/paste entre host e VM funciona
- instalar sublime
- add terminal to favorites
- dark mode

## 6. Instalar dependências de compilação
`sudo apt update`

`sudo apt install build-essential libncurses-dev libssl-dev libelf-dev bison flex bc dwarves kmod cpio linux-source`

- Confirmar instalação:
`dpkg -l build-essential libncurses-dev libssl-dev libelf-dev bison flex bc dwarves kmod cpio linux-source`
- Todos devem aparecer com `ii` na primeira coluna

## 7. Desativar suspensão automática da VM
`gsettings set org.gnome.settings-daemon.plugins.power sleep-inactive-ac-timeout 0`

`gsettings set org.gnome.desktop.session idle-delay 0`

## 8. Obter e extrair o código-fonte do kernel
`mkdir ~/kernel && tar -xf /usr/src/linux-source-5.10.tar.xz -C ~/kernel`

## 9. Configurar o kernel
`cp /boot/config-$(uname -r) ~/kernel/linux-source-5.10/.config`

`cd ~/kernel/linux-source-5.10`

`make oldconfig`

## 10. Corrigir erro de chaves de assinatura
`scripts/config --set-str SYSTEM_TRUSTED_KEYS ""`

`scripts/config --set-str SYSTEM_REVOCATION_KEYS ""`

`scripts/config --disable MODULE_SIG_KEY`

`make oldconfig`
- Quando perguntar sobre MODULE_SIG_KEY: digitar um espaço e pressionar Enter
- Para demais perguntas: pressionar Enter para aceitar o padrão

## 11. Compilar o kernel
`make -j$(nproc) 2>&1 | tee ~/kernel-build.log`

## 12. Verificar se a compilação foi bem sucedida
`echo $?`

Deve retornar 0

`grep "ready" ~/kernel-build.log`

`ls arch/x86/boot/bzImage`

- Ambos devem retornar resultados positivos antes de prosseguir

## 13. Instalar o kernel compilado
`sudo make modules_install`

`sudo make install`

## 14. Reiniciar e confirmar novo kernel
`sudo reboot`

- No GRUB selecionar o novo kernel
- Após boot confirmar:
`uname -r`
- Deve retornar `5.10.257`

## 15. Snapshot B — Kernel original recompilado e funcionando

## 16. Inicializar repositório git e instalar sublime
- Reescrever .gitignore
- inicializar git
`cd ~/kernel/linux-source-5.10`


`git init`

`git add -A`

`git commit -m "baseline: kernel original sem modificacoes"`

## 17. Criar file_monitor.c
`touch kernel/file_monitor.c`

## 18. Registrar file_monitor.c no Makefile
- Abrir `kernel/Makefile`
- Localizar a linha com `tracepoint.o`
- Adicionar abaixo: `obj-y += file_monitor.o`

## 19. Snapshot C — Ambiente pronto para modificações

---


## Planejamento
## 1. Configurar o Ambiente

- Instale o VirtualBox (ou QEMU/KVM)
- Crie uma VM com Debian 11 (Bullseye) — recomendado: 4GB RAM, 40GB disco, 2+ CPUs
- **Tire um snapshot antes de qualquer alteração no kernel** — isso é indispensável

## 2. Instalar Dependências de Compilação

Dentro da VM, instale os pacotes necessários para compilar o kernel:
`build-essential`, `libncurses-dev`, `libssl-dev`, `libelf-dev`, `bison`, `flex`, `bc`, `dwarves`, `kmod`, `cpio`

## 3. Obter o Código-Fonte do Kernel

- Use o pacote `linux-source` disponível nos repositórios do Debian 11
- O fonte ficará disponível em `/usr/src/` como um `.tar.xz`
- Extraia para um diretório de trabalho (ex: `~/kernel/`)

## 4. Configurar o Kernel

- Copie a configuração do kernel atualmente em uso como ponto de partida
  - Ela fica em `/boot/config-$(uname -r)`
- Use `make oldconfig` ou `make localmodconfig` para partir de uma config funcional
- Evite `make menuconfig` do zero — desnecessariamente complexo para este projeto

## 5. Fazer uma Compilação Limpa Antes de Alterar Qualquer Coisa

- Compile o kernel **sem modificações** primeiro
- Isso valida que seu ambiente está funcionando corretamente
- Instale o kernel compilado e confirme que o sistema ainda inicializa normalmente
- **Tire outro snapshot aqui**
- Vamos fazer 3 snapshots: Snapshot A (VM recém-instalada); Snapshot B (Kernel original recompilado e funcionando); Snapshot C (Antes de começar a alterar fs/open.c ou adicionar syscalls)

## 6. Estudar os Arquivos Relevantes Antes de Escrever Código

Familiarize-se com a estrutura antes de mexer em qualquer coisa:
- `fs/open.c` — onde a abertura de arquivos é processada
- `arch/x86/entry/syscalls/syscall_64.tbl` — tabela de syscalls
- `include/linux/syscalls.h` — declarações de syscalls
- `kernel/Makefile` — onde novos arquivos do kernel são registrados

## 7. Estudar as Ferramentas de Diagnóstico

Antes de escrever uma linha de código do subsistema, pratique com:
- `dmesg` — leitura de mensagens do kernel
- `printk` — como o kernel "imprime" para debug
- `strace` — rastrear syscalls de um processo no userspace

---

## Ordem de Leitura Recomendada

1. https://kernelnewbies.org/AddingSyscalls
2. https://www.kernel.org/doc/html/latest/process/adding-syscalls.html
3. https://blog.packagecloud.io/the-definitive-guide-to-linux-system-calls/
