Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents circleWidget As AxmultipleaxLib.AxQAxWidget2
    Friend WithEvents resetButton As AxwrapperaxLib.AxQPushButton
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(Form1))
        Me.circleWidget = New AxmultipleaxLib.AxQAxWidget2()
        Me.resetButton = New AxwrapperaxLib.AxQPushButton()
        CType(Me.circleWidget, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.resetButton, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'circleWidget
        '
        Me.circleWidget.Anchor = (((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
                    Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right)
        Me.circleWidget.Enabled = True
        Me.circleWidget.Location = New System.Drawing.Point(8, 8)
        Me.circleWidget.Name = "circleWidget"
        Me.circleWidget.OcxState = CType(resources.GetObject("circleWidget.OcxState"), System.Windows.Forms.AxHost.State)
        Me.circleWidget.Size = New System.Drawing.Size(280, 216)
        Me.circleWidget.TabIndex = 0
        '
        'resetButton
        '
        Me.resetButton.Anchor = (System.Windows.Forms.AnchorStyles.Bottom Or System.Windows.Forms.AnchorStyles.Right)
        ' VB is case insensitive, but our C++ controls are not.
        ' Me.resetButton.enabled = True
        Me.resetButton.Location = New System.Drawing.Point(184, 240)
        Me.resetButton.Name = "resetButton"
        Me.resetButton.OcxState = CType(resources.GetObject("resetButton.OcxState"), System.Windows.Forms.AxHost.State)
        Me.resetButton.Size = New System.Drawing.Size(104, 24)
        Me.resetButton.TabIndex = 1
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(292, 273)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.resetButton, Me.circleWidget})
        Me.Name = "Form1"
        Me.Text = "Form1"
        CType(Me.circleWidget, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.resetButton, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub circleWidget_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles circleWidget.ClickEvent
        Me.circleWidget.lineWidth = Me.circleWidget.lineWidth + 1
    End Sub

    Private Sub resetButton_clicked(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles resetButton.clicked
        Me.circleWidget.lineWidth = 1
        Me.resetButton.setFocus()
    End Sub
End Class
