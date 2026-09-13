# Fish completion for VoltBill
# Lead Architect: Akshar Miyani

complete -c voltbill -f

# Options
complete -c voltbill -l demo -d "Seed realistic demo records & launch dashboard"
complete -c voltbill -l seed -d "Seed realistic demo records & launch dashboard"
complete -c voltbill -l export -d "Export all consumers and bills to CSV and exit"
complete -c voltbill -l backup -d "Create a full timestamped JSON backup snapshot"
complete -c voltbill -l batch -d "Execute batch billing run for all active consumers"
complete -c voltbill -s v -l version -d "Display developer info and version"
complete -c voltbill -s h -l help -d "Show help message"

# Subcommands
complete -c voltbill -n "__fish_use_subcommand" -a bill -d "Generate invoice directly from terminal: <id> <reading>"
complete -c voltbill -n "__fish_use_subcommand" -a pay -d "Process payment: <id> <amount> [mode]"
complete -c voltbill -n "__fish_use_subcommand" -a status -d "Show instant balance and meter status: <id>"
complete -c voltbill -n "__fish_use_subcommand" -a calc -d "Instant tariff simulation: <units> [cat] [solar]"
complete -c voltbill -n "__fish_use_subcommand" -a qr -d "Synthesize & display real scannable QR matrix"
complete -c voltbill -n "__fish_use_subcommand" -a scada -d "Launch real-time SCADA substation grid monitor"
complete -c voltbill -n "__fish_use_subcommand" -a grid -d "Alias for scada telemetry monitor"
complete -c voltbill -n "__fish_use_subcommand" -a ufls -d "Dynamic under-frequency load shedding defense simulator"
complete -c voltbill -n "__fish_use_subcommand" -a modbus -d "Inspect & decode industrial Modbus RTU telemetry"
