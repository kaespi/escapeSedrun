<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Form1))
        Me.Button1 = New System.Windows.Forms.Button()
        Me.lblPwd = New System.Windows.Forms.Label()
        Me.lblHint = New System.Windows.Forms.Label()
        Me.txtPwd = New System.Windows.Forms.TextBox()
        Me.videoPlayer = New AxWMPLib.AxWindowsMediaPlayer()
        Me.lblHixon = New System.Windows.Forms.Label()
        CType(Me.videoPlayer, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Button1
        '
        Me.Button1.FlatStyle = System.Windows.Forms.FlatStyle.Flat
        Me.Button1.Location = New System.Drawing.Point(34, 24)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(30, 25)
        Me.Button1.TabIndex = 100
        Me.Button1.TabStop = False
        Me.Button1.Text = "Exit"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'lblPwd
        '
        Me.lblPwd.AutoSize = True
        Me.lblPwd.Font = New System.Drawing.Font("Microsoft Sans Serif", 20.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblPwd.ForeColor = System.Drawing.Color.White
        Me.lblPwd.Location = New System.Drawing.Point(126, 150)
        Me.lblPwd.Name = "lblPwd"
        Me.lblPwd.Size = New System.Drawing.Size(127, 31)
        Me.lblPwd.TabIndex = 1
        Me.lblPwd.Text = "Passwort"
        '
        'lblHint
        '
        Me.lblHint.AutoSize = True
        Me.lblHint.Font = New System.Drawing.Font("Microsoft Sans Serif", 16.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblHint.ForeColor = System.Drawing.Color.White
        Me.lblHint.Location = New System.Drawing.Point(126, 221)
        Me.lblHint.Name = "lblHint"
        Me.lblHint.Size = New System.Drawing.Size(466, 26)
        Me.lblHint.TabIndex = 2
        Me.lblHint.Text = "(Hinweis zum Passwort: Geheimnisse der Zeit)"
        '
        'txtPwd
        '
        Me.txtPwd.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.txtPwd.Font = New System.Drawing.Font("Microsoft Sans Serif", 20.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtPwd.ForeColor = System.Drawing.Color.Red
        Me.txtPwd.Location = New System.Drawing.Point(298, 150)
        Me.txtPwd.Name = "txtPwd"
        Me.txtPwd.Size = New System.Drawing.Size(307, 38)
        Me.txtPwd.TabIndex = 0
        Me.txtPwd.Text = "Passwort hier eingeben"
        '
        'videoPlayer
        '
        Me.videoPlayer.Enabled = True
        Me.videoPlayer.Location = New System.Drawing.Point(368, 319)
        Me.videoPlayer.Name = "videoPlayer"
        Me.videoPlayer.OcxState = CType(resources.GetObject("videoPlayer.OcxState"), System.Windows.Forms.AxHost.State)
        Me.videoPlayer.Size = New System.Drawing.Size(133, 52)
        Me.videoPlayer.TabIndex = 101
        Me.videoPlayer.Visible = False
        '
        'lblHixon
        '
        Me.lblHixon.AutoSize = True
        Me.lblHixon.Font = New System.Drawing.Font("Microsoft Sans Serif", 36.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.lblHixon.ForeColor = System.Drawing.Color.White
        Me.lblHixon.Location = New System.Drawing.Point(253, 87)
        Me.lblHixon.Name = "lblHixon"
        Me.lblHixon.Size = New System.Drawing.Size(482, 55)
        Me.lblHixon.TabIndex = 102
        Me.lblHixon.Text = "Dr. Hixon's Computer"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BackColor = System.Drawing.Color.Black
        Me.ClientSize = New System.Drawing.Size(779, 457)
        Me.Controls.Add(Me.lblHixon)
        Me.Controls.Add(Me.videoPlayer)
        Me.Controls.Add(Me.txtPwd)
        Me.Controls.Add(Me.lblHint)
        Me.Controls.Add(Me.lblPwd)
        Me.Controls.Add(Me.Button1)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.WindowState = System.Windows.Forms.FormWindowState.Maximized
        CType(Me.videoPlayer, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents lblPwd As System.Windows.Forms.Label
    Friend WithEvents lblHint As System.Windows.Forms.Label
    Friend WithEvents txtPwd As System.Windows.Forms.TextBox
    Friend WithEvents videoPlayer As AxWMPLib.AxWindowsMediaPlayer
    Friend WithEvents lblHixon As System.Windows.Forms.Label

End Class
