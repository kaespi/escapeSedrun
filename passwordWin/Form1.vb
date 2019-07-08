Public Class Form1

    Dim closeButtonPressed = False

    Declare Function SetWindowsHookEx Lib "user32" Alias "SetWindowsHookExA" (ByVal idHook As Integer, ByVal lpfn As LowLevelKeyboardProcDelegate, ByVal hMod As IntPtr, ByVal dwThreadId As Integer) As IntPtr
    Declare Function UnhookWindowsHookEx Lib "user32" Alias "UnhookWindowsHookEx" (ByVal hHook As IntPtr) As Boolean
    Declare Function CallNextHookEx Lib "user32" Alias "CallNextHookEx" (ByVal hHook As IntPtr, ByVal nCode As Integer, ByVal wParam As Integer, ByRef lParam As KBDLLHOOKSTRUCT) As Integer
    Delegate Function LowLevelKeyboardProcDelegate(ByVal nCode As Integer, ByVal wParam As Integer, ByRef lParam As KBDLLHOOKSTRUCT) As Integer

    Const WH_KEYBOARD_LL As Integer = 13

    Structure KBDLLHOOKSTRUCT
        Dim vkCode As Integer
        Dim scanCode As Integer
        Dim flags As Integer
        Dim time As Integer
        Dim dwExtraInfo As Integer
    End Structure

    Dim intLLKey As IntPtr

    Public Sub New()
        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        Me.FormBorderStyle = Windows.Forms.FormBorderStyle.None
    End Sub

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        intLLKey = SetWindowsHookEx(WH_KEYBOARD_LL, AddressOf LowLevelKeyboardProc, IntPtr.Zero, 0)
        Dim formW = Me.Size.Width
        Dim formH = Me.Size.Height
        Dim buttonW = Button1.Width
        Dim buttonH = Button1.Height

        Button1.Location = New Point((formW - buttonW) / 2, (formH - buttonH) / 2)

    End Sub

    Private Sub Form1_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        UnhookWindowsHookEx(intLLKey)
    End Sub

    Private Function LowLevelKeyboardProc(ByVal nCode As Integer, ByVal wParam As Integer, ByRef lParam As KBDLLHOOKSTRUCT) As Integer
        Dim blnEat As Boolean = False

        Select Case wParam
            Case 256, 257, 260, 261
                'Alt+Tab, Alt+Esc, Ctrl+Esc, Windows Key    
                blnEat = ((lParam.vkCode = 9) AndAlso (lParam.flags = 32)) Or _
                ((lParam.vkCode = 27) AndAlso (lParam.flags = 32)) Or _
                ((lParam.vkCode = 27) AndAlso (lParam.flags = 0)) Or _
               ((lParam.vkCode = 91) AndAlso (lParam.flags = 1)) Or _
                ((lParam.vkCode = 92) AndAlso (lParam.flags = 1))
        End Select

        If blnEat = True Then
            Return 1
        Else
            Return CallNextHookEx(IntPtr.Zero, nCode, wParam, lParam)
        End If
    End Function

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        closeButtonPressed = True
        Close()
    End Sub

    Private Sub formKeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles MyBase.KeyDown
        e.Handled = True
    End Sub

    Private Sub buttonKeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Button1.KeyDown
        e.Handled = True
    End Sub

    Protected Overrides Function ProcessCmdKey(ByRef msg As Message, ByVal keyData As Keys) As Boolean

        Return True

    End Function

    Private Sub Form1_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        If (e.CloseReason = CloseReason.UserClosing And closeButtonPressed = False) Then
            e.Cancel = True
        End If
    End Sub
End Class
