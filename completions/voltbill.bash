# Bash completion for VoltBill
# Lead Architect: Akshar Miyani

_voltbill_completions() {
    local cur prev opts commands
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    opts="--demo --seed --export --backup --batch --version -v --help -h"
    commands="bill pay status calc qr scada grid ufls modbus"

    if [ "$COMP_CWORD" -eq 1 ]; then
        if [[ "$cur" == -* ]]; then
            COMPREPLY=( $(compgen -W "$opts" -- "$cur") )
        else
            COMPREPLY=( $(compgen -W "$commands" -- "$cur") )
        fi
        return 0
    fi

    case "$prev" in
        calc)
            # category hint: 0=Domestic 1=Commercial 2=Industrial 3=Agricultural
            COMPREPLY=( $(compgen -W "0 1 2 3" -- "$cur") )
            return 0
            ;;
        pay)
            # payment mode: 0=Cash 1=UPI 2=Card 3=NetBank
            COMPREPLY=( $(compgen -W "0 1 2 3" -- "$cur") )
            return 0
            ;;
        *)
            ;;
    esac
}

complete -F _voltbill_completions voltbill
