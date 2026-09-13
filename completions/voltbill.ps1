# PowerShell Argument Completer for VoltBill
# Lead Architect: Akshar Miyani
# Usage: Add to $PROFILE: . (Join-Path (Get-Command voltbill).Source "..\..\completions\voltbill.ps1")

Register-ArgumentCompleter -Native -CommandName voltbill -ScriptBlock {
    param($wordToComplete, $commandAst, $cursorPosition)

    $subcommands = @('bill', 'pay', 'status', 'calc', 'qr', 'scada', 'grid', 'ufls', 'modbus')
    $options = @('--demo', '--seed', '--export', '--backup', '--batch', '--version', '-v', '--help', '-h')

    $elements = $commandAst.Elements
    if ($elements.Count -le 2) {
        $matches = ($subcommands + $options) | Where-Object { $_ -like "$wordToComplete*" }
        foreach ($m in $matches) {
            [System.Management.Automation.CompletionResult]::new($m, $m, 'ParameterValue', $m)
        }
    }
}
