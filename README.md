# Primeiros Passos — Projeto de Monitoramento de Kernel

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
