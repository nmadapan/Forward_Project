# -*- coding: utf-8 -*-
"""
Created on Tue Jan  1 20:37:56 2019

@author: Jyothsna P B
"""

import wx
import wx.xrc
import cv2
import numpy as np

class AnnotationTool2 ( wx.Frame ):
    
    def __init__( self, parent, video_path, write_path, VideoDisplaySize ):
        # Define surgeme list and initial parameters
        self.WindowWidth = VideoDisplaySize[0] + 440
        self.WindowHeight = VideoDisplaySize[1] + 190
        
        self.ImWidth = VideoDisplaySize[0]
        self.ImHeight = VideoDisplaySize[1]
        self.surgeme = displaylist
        self.surgemelist = ['S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7']
        self.s_f = ['Success', 'Fail']
        self.FrameNumber = 0
        self.FrameTime = 33
        self.video_path = video_path
        self.write_path = write_path
        self.OneRow = []
        self.IndexSurgeme = 0
        self.IndexRecord = 0
        self.IndexSF = 0     
        self.OneAnnotation = ''
        self.Index = 0
                
        wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = wx.EmptyString, pos = wx.DefaultPosition, \
                     size = wx.Size( self.WindowWidth, self.WindowHeight ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
        self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )   
                
        #Define the framework
        bSizer1 = wx.BoxSizer( wx.VERTICAL )        
        bSizer2 = wx.BoxSizer( wx.HORIZONTAL )
        
        bSizer2.SetMinSize( wx.Size( -1,20 ) ) 
        self.m_load = wx.Button( self, wx.ID_ANY, u"Load", wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer2.Add( self.m_load, 1, wx.ALL|wx.EXPAND, 5 )
        
        self.Inform_bar = wx.TextCtrl( self, wx.ID_ANY, u"Information is shown here", \
                                wx.DefaultPosition, wx.DefaultSize, wx.TE_CENTRE|wx.TE_READONLY )
        self.Inform_bar.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_MENU ) )        
        bSizer2.Add( self.Inform_bar, 3, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )
                
        bSizer1.Add( bSizer2, 0, wx.EXPAND, 5 )        
        sbSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"" ), wx.VERTICAL )        
        bSizer4 = wx.BoxSizer( wx.HORIZONTAL )
        
        self.m_bitmap = wx.StaticBitmap( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.NullBitmap, \
                                  wx.DefaultPosition, wx.Size(self.ImWidth,self.ImHeight ), 0 )
        bSizer4.Add( self.m_bitmap, 0, wx.ALL, 5 )
        
        bSizer5 = wx.BoxSizer( wx.VERTICAL )
        
        bSizer5.SetMinSize( wx.Size( 210,-1 ) ) 
        self.m_toggleBtn2 = wx.ToggleButton( sbSizer3.GetStaticBox(), wx.ID_ANY,\
                                      u"Choose Start Frame", wx.Point( -1,-1 ), wx.Size( 180,-1 ), 0 )
        self.m_toggleBtn2.SetValue( True ) 
        bSizer5.Add( self.m_toggleBtn2, 1, wx.ALIGN_CENTER|wx.ALL, 5 )
        
        self.m_staticText = wx.StaticText( sbSizer3.GetStaticBox(), wx.ID_ANY, \
                                    u"\nChoose Surgeme :", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText.Wrap( -1 )
        bSizer5.Add( self.m_staticText, 0, wx.ALL, 5 )
        
        m_listBoxChoices = self.surgemelist
        self.m_listBox = wx.ListBox( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, \
                              wx.Size( 120,-1 ), m_listBoxChoices, wx.LB_EXTENDED|wx.LB_HSCROLL|wx.LB_NEEDED_SB|wx.LB_SINGLE )
        self.m_listBox.SetFont( wx.Font( 16, 70, 90, 90, False, "Century" ) )
        self.m_listBox.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_BTNFACE ) )
        
        bSizer5.Add( self.m_listBox, 4, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        
        self.m_staticText2 = wx.StaticText( sbSizer3.GetStaticBox(), wx.ID_ANY, \
                                     u"\nSuccess or Fail :", wx.DefaultPosition, wx.DefaultSize, 0 )
        self.m_staticText2.Wrap( -1 )
        bSizer5.Add( self.m_staticText2, 0, wx.ALL, 5 )
        
        m_choiceChoices = self.s_f
        self.m_choice = wx.Choice( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, m_choiceChoices, 0 )
        self.m_choice.SetSelection( 0 )
        bSizer5.Add( self.m_choice, 0, wx.ALIGN_CENTER_HORIZONTAL|wx.ALL, 5 )
        
        self.m_Anno = wx.Button( sbSizer3.GetStaticBox(), wx.ID_ANY, u"Create Annotation :", \
                          wx.DefaultPosition, wx.Size( 180,-1 ), 0 )
        bSizer5.Add( self.m_Anno, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        
        self.m_Annotext = wx.TextCtrl( sbSizer3.GetStaticBox(), wx.ID_ANY, \
                                wx.EmptyString, wx.DefaultPosition, wx.Size( 180,-1 ), wx.TE_CENTRE)
        self.m_Annotext.SetFont( wx.Font( 14, 70, 90, 90, False, "Century" ) )
        self.m_Annotext.SetForegroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_BACKGROUND ) )
        self.m_Annotext.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_MENU ) )
        self.m_Annotext.SetMinSize( wx.Size( 180,-1 ) )
        self.m_Annotext.SetMaxSize( wx.Size( 180,-1 ) )        
        bSizer5.Add( self.m_Annotext, 1, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        
        self.m_Write = wx.Button( sbSizer3.GetStaticBox(), wx.ID_ANY, u"Write To File", wx.Point( -1,-1 ), wx.Size( 140,30 ), 0 )
        self.m_Write.SetMinSize( wx.Size( 130,30 ) )
        self.m_Write.SetMaxSize( wx.Size( 160,40 ) )    
        bSizer5.Add( self.m_Write, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        
             
        self.m_delete = wx.ToggleButton( sbSizer3.GetStaticBox(), wx.ID_ANY, \
                            u"Go to Start Frame", wx.DefaultPosition, wx.Size( 140,30 ), 0 )
        self.m_toggleBtn2.SetValue( True )
        self.m_delete.SetMinSize( wx.Size( 130,30 ) )
        self.m_delete.SetMaxSize( wx.Size( 160,40 ) )        
        bSizer5.Add( self.m_delete, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        
        bSizer4.Add( bSizer5, 1, wx.EXPAND, 5 )
        
        sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( sbSizer3.GetStaticBox(), wx.ID_ANY, \
                                             u"All annotations" ), wx.VERTICAL )
        
        sbSizer2.SetMinSize( wx.Size( 180,-1 ) )     
               
        m_listboxChoices = self.surgeme
        self.m_listbox = wx.ListBox( sbSizer3.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, \
                              wx.Size( 180,-1 ), m_listboxChoices, wx.LB_EXTENDED|wx.LB_HSCROLL|wx.LB_NEEDED_SB|wx.LB_SINGLE )
        self.m_listbox.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), 70, 90, 90, False, "Calibri") )
        self.m_listbox.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_BTNFACE ) )
        
        sbSizer2.Add( self.m_listbox, 4, wx.ALL|wx.EXPAND, 5 )
                
        bSizer4.Add( sbSizer2, 1, wx.EXPAND, 5 )        
        sbSizer3.Add( bSizer4, 1, wx.EXPAND, 5 )        
        bSizer1.Add( sbSizer3, 15, wx.EXPAND, 5 )        
        bSizer5 = wx.BoxSizer( wx.VERTICAL )
        
        bSizer5.SetMinSize( wx.Size( -1,50 ) ) 
        self.m_slider = wx.Slider( self, wx.ID_ANY, 0, 0, 4000, wx.DefaultPosition, wx.Size( -1,40 ), wx.SL_LABELS|wx.SL_TOP )
        self.m_slider.SetMinSize( wx.Size( -1,30 ) )
        self.m_slider.SetMaxSize( wx.Size( -1,50 ) )
        
        bSizer5.Add( self.m_slider, 1, wx.ALL|wx.EXPAND, 5 )
        
        bSizer6 = wx.BoxSizer( wx.HORIZONTAL )
        
        bSizer6.SetMinSize( wx.Size( -1,20 ) ) 
        self.m_buttonSlow = wx.Button( self, wx.ID_ANY, u"0.5X", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonSlow, 1, wx.ALL, 5 )
        
        self.m_buttonBefore = wx.Button( self, wx.ID_ANY, u"Last Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonBefore, 1, wx.ALL, 5 )
        
        self.m_toggleBtn = wx.ToggleButton( self, wx.ID_ANY, u"Pause", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_toggleBtn, 2, wx.ALL, 5 )
        
        self.m_buttonNext = wx.Button( self, wx.ID_ANY, u"Next Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonNext, 1, wx.ALL, 5 )
        
        self.m_buttonFast = wx.Button( self, wx.ID_ANY, u"2X", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonFast, 1, wx.ALL, 5 )        
        
        bSizer5.Add( bSizer6, 1, wx.EXPAND, 5 )        
        bSizer1.Add( bSizer5, 0, wx.EXPAND, 5 )        
        
        self.SetSizer( bSizer1 )
        self.Layout()
        self.m_timer1 = wx.Timer()
        self.m_timer1.SetOwner( self, wx.ID_ANY )
        
        self.Centre( wx.BOTH )
        
        # Connect Events
        self.m_load.Bind( wx.EVT_BUTTON, self.OnLoad )
        self.m_toggleBtn2.Bind( wx.EVT_TOGGLEBUTTON, self.ToggleSaveFrame )
        self.m_listBox.Bind( wx.EVT_LISTBOX, self.SurgemeChosed )
        self.m_listbox.Bind( wx.EVT_LISTBOX, self.RecordChosed )
        self.m_choice.Bind( wx.EVT_CHOICE, self.s_fChosed )
        self.m_Anno.Bind( wx.EVT_BUTTON, self.CreateAnnotation )
        self.m_Write.Bind( wx.EVT_BUTTON, self.SurgemeWrite )
        self.m_delete.Bind( wx.EVT_TOGGLEBUTTON, self.RecordDelete )
        self.m_slider.Bind( wx.EVT_SCROLL, self.OnSliderScroll )
        self.m_buttonSlow.Bind( wx.EVT_BUTTON, self.OnSlow )
        self.m_buttonBefore.Bind( wx.EVT_BUTTON, self.LastFrame )
        self.m_toggleBtn.Bind( wx.EVT_TOGGLEBUTTON, self.Play_Pause )
        self.m_buttonNext.Bind( wx.EVT_BUTTON, self.NextFrame )
        self.m_buttonFast.Bind( wx.EVT_BUTTON, self.OnFast )
        self.Bind( wx.EVT_TIMER, self.OnTime, id = wx.ID_ANY )
        
        self.m_toggleBtn.Bind( wx.EVT_KEY_DOWN, self.KeyboardEvent )
        self.m_buttonNext.Bind( wx.EVT_KEY_DOWN, self.KeyboardEvent )
        self.m_buttonBefore.Bind( wx.EVT_KEY_DOWN, self.KeyboardEvent )
        self.Bind( wx.EVT_KEY_DOWN, self.KeyboardEvent )            
        
        #Set flags
        self.PAUSE_FLAG = False
        self.PROCESSING_FLAG = False
        self.SurgemeChosed_FLAG = False
        self.S_fChosed_FLAG = False
        self.RecordChosed_FLAG = False
        
    def __del__( self ):
        self.m_timer1.Stop()
        pass    
        
    # Virtual event handlers, overide them in your derived class
    def OnLoad( self, event ):
        self.videoCapture = cv2.VideoCapture(self.video_path)
        if(self.videoCapture == None):
            wx.SafeShowMessage('start', 'Open Failed')
            return
        self.TotalFrame = self.videoCapture.get(cv2.CAP_PROP_FRAME_COUNT)
        self.m_slider.SetMax(int(self.TotalFrame))
        self.fps = self.videoCapture.get(cv2.CAP_PROP_FPS)
        self.FrameTime = 1000 / self.fps
        self.m_timer1.Start(self.FrameTime)
        self.Inform_bar.SetValue('Video Loaded : ' + self.video_path)
        self.PROCESSING_FLAG = True
        event.Skip()
    
    # Select the start and end frame of a surgeme
    def ToggleSaveFrame( self, event ):
        if not self.RecordChosed_FLAG and self.PROCESSING_FLAG:
            self.EndFrame = event.GetEventObject().GetValue()
            if not self.EndFrame and not self.OneRow:
                FN = str(int(self.FrameNumber))
                self.OneRow.append(FN)
                event.GetEventObject().SetLabel("Choose End Frame")
                self.Inform_bar.SetValue('Annotation Starts at Frame : ' + FN + '. Please choose the end frame')
            elif self.EndFrame and len(self.OneRow) == 1:
                if int(self.FrameNumber) != int(self.OneRow[0]):
                    FN = str(int(self.FrameNumber))
                    self.OneRow.append(FN)
                    event.GetEventObject().SetLabel("Choose Start Frame")
                    self.Inform_bar.SetValue('Annotation ends at Frame : ' + FN + '. Please choose the surgeme')
                else:
                    self.Inform_bar.SetValue('Do not choose the same frame!')                
        else:
            self.Inform_bar.SetValue('Please choose record to edit.')
        event.Skip()
    
    # Write an annotation to file and display it on annotation window
    def SurgemeWrite( self, event ):
        if self.PROCESSING_FLAG:
            # if the complete annotation is not created
            if len(self.OneRow) == 2 and self.IndexSurgeme:
                self.OneRow.append('S' + str(self.IndexSurgeme))
                self.OneRow.append('False' if self.IndexSF else 'True')  
                for item in self.OneRow:
                    self.OneAnnotation += (item + ' ')
                self.surgeme[self.IndexRecord] = self.OneAnnotation
                self.MyFileWriting()
                # initilize the recording buffer
                self.OneRow = []
                self.OneAnnotation = ''
            # else if the complete annotation was created by 'create annotation' function
            elif len(self.OneRow) == 4:
                self.OneAnnotation = self.m_Annotext.GetLineText(0)
                self.surgeme[self.IndexRecord] = self.OneAnnotation
                self.MyFileWriting()  
                self.OneRow = []
                self.OneAnnotation = ''                  
            else:
                self.Inform_bar.SetValue('please choose a surgeme')
        else:
            self.Inform_bar.SetValue('Please load the video!')
        event.Skip()
        
    # record what surgeme is chosed
    def SurgemeChosed( self, event ):
        IndexSurgeme = self.m_listBox.GetSelections()
        if len(IndexSurgeme) == 1:
            self.IndexSurgeme = int(IndexSurgeme[0]) + 1
        else:
            self.Inform_bar.SetValue('Please choose a single surgeme!')
        event.Skip()
        
    def RecordChosed( self, event ):
        if self.PROCESSING_FLAG:
            self.Index = self.m_listbox.GetSelections()
            if len(self.Index) == 1:
                self.IndexRecord = int(self.Index[0])
            else:
                self.Inform_bar.SetValue('Please choose a single Record!')
        else:
            self.Inform_bar.SetValue('Please load the video!')                
        event.Skip()
    
    # record the s/f mark of the surgeme
    def s_fChosed( self, event ):
        self.IndexSF = self.m_choice.GetSelection()
        event.Skip()
        
    # create an complete annotation and display it
    def CreateAnnotation( self, event ):
        if self.PROCESSING_FLAG:
            if len(self.OneRow) == 2 and self.IndexSurgeme:
                self.OneRow.append('S' + str(self.IndexSurgeme))
                self.OneRow.append('False' if self.IndexSF else 'True')
                for item in self.OneRow:
                    self.OneAnnotation += (item + ' ')
                self.m_Annotext.SetValue(self.OneAnnotation)
            else:
                self.Inform_bar.SetValue('please choose a surgeme')
        else:
            self.Inform_bar.SetValue('Please load the video!')
        event.Skip()
    
    # delete the last annotation record
    def RecordDelete( self, event ):
        if self.PROCESSING_FLAG:
            self.EndFrame = event.GetEventObject().GetValue()
            if self.EndFrame and len(self.Index) == 1:
                self.RecordChosed_FLAG = True
                self.FrameNumber = int(self.surgeme[self.IndexRecord].split()[0])
                self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
                success, self.CurrentFrame = self.videoCapture.read()
                if(success) :
                    self.MyImshow()
                event.GetEventObject().SetLabel("Go to End Frame")
                self.Inform_bar.SetValue('Record has been selected to edit. Annotation Starts at Frame : ' + str(self.FrameNumber) + '. Please choose to go to end frame')
            if not self.EndFrame and len(self.Index) == 1:
                self.RecordChosed_FLAG = False
                self.FrameNumber = int(self.surgeme[self.IndexRecord].split()[1])
                self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
                success, self.CurrentFrame = self.videoCapture.read()
                if(success) :
                    self.MyImshow()
                event.GetEventObject().SetLabel("Go to Start Frame")
                self.Inform_bar.SetValue('Annotation ends at Frame : ' + str(self.FrameNumber) + '. Please choose the start frame you want.')
        else:
            self.Inform_bar.SetValue('Please load the video!')
        event.Skip()
    
    # when the slider is used, set new framenumber and display the corresponding image
    def OnSliderScroll( self, event ):
        if self.PROCESSING_FLAG:
            self.FrameNumber = self.m_slider.GetValue()
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES, self.FrameNumber)
            success, self.CurrentFrame = self.videoCapture.read()
            if(success):
                self.MyImshow()
        else:
            event.Skip()
    
    # display the last frame
    def LastFrame( self, event ):
        try:
            self.FrameNumber -= 1
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
        except AttributeError:
            self.Inform_bar.SetValue('Please load the video!')
        else:
            success, self.CurrentFrame = self.videoCapture.read()
            if(success) :
                self.MyImshow()
        event.Skip()
    
    # toggle the playing and pausing condition of the video
    def Play_Pause( self, event ):
        if self.PROCESSING_FLAG:
            self.PAUSE_FLAG = event.GetEventObject().GetValue()
            if self.PAUSE_FLAG:
                self.m_timer1.Stop()
                event.GetEventObject().SetLabel("Play")
            else:
                self.m_timer1.Start(self.FrameTime)
                event.GetEventObject().SetLabel("Pause")
        else:
            self.Inform_bar.SetValue('Please load the video!')
        event.Skip()
    
    def NextFrame( self, event ):
        try:
            self.FrameNumber += 1
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
        except AttributeError:
            self.Inform_bar.SetValue('Please load the video!')
        else:
            success, self.CurrentFrame = self.videoCapture.read()
            if(success) :
                self.MyImshow()
        event.Skip()
    
    # slow down the fps to a half
    def OnSlow( self, event ):
        if self.FrameTime <= 200:
            self.FrameTime = self.FrameTime * 2
            if not self.PAUSE_FLAG: self.m_timer1.Start(self.FrameTime)
            self.Inform_bar.SetValue('Current fps:' + str(1000/self.FrameTime))
        else:
            self.Inform_bar.SetValue('Minimum fps reached!')
        event.Skip()    
    
    # set the fps doubly
    def OnFast( self, event ):
        if self.FrameTime >= 10:
            self.FrameTime = self.FrameTime * 0.5
            if not self.PAUSE_FLAG: self.m_timer1.Start(self.FrameTime)
            self.Inform_bar.SetValue('Current fps:' + str(1000/self.FrameTime))
        else:
            self.Inform_bar.SetValue('Maximum fps reached!')
        event.Skip()
    
    # execute for every timer value 
    def OnTime( self, event ):
        if self.PROCESSING_FLAG:
            success, self.CurrentFrame = self.videoCapture.read()
            self.FrameNumber = self.videoCapture.get(cv2.CAP_PROP_POS_FRAMES)
            if(success):
                self.MyImshow()
        else:
            event.Skip()
            
    def KeyboardEvent( self, event ):
        keycode = event.GetKeyCode()
        if keycode in [wx.WXK_LEFT, wx.WXK_RIGHT] and self.PROCESSING_FLAG:
            self.FrameNumber = self.FrameNumber - 1 if keycode == wx.WXK_LEFT else self.FrameNumber + 1
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
            success, self.CurrentFrame = self.videoCapture.read()
            if(success):
                self.MyImshow()
        event.Skip()
            
    # Event handle defined, my functions below
    def MyImshow(self):
        self.CurrentFrame = cv2.resize(self.CurrentFrame, (self.ImWidth, self.ImHeight), interpolation = cv2.INTER_CUBIC)
        image = cv2.cvtColor(self.CurrentFrame, cv2.COLOR_BGR2RGB)
        pic = wx.Bitmap.FromBuffer(self.ImWidth, self.ImHeight, image) 
        self.m_bitmap.SetBitmap(pic)
        self.m_slider.SetValue(int(self.FrameNumber))
        
    def MyFileWriting(self):
        f = open(self.write_path, 'w')
        for annot in self.surgeme:
            f.write(str(annot)+'\n')
        f.close()
        self.m_listbox.Clear()
        self.m_listbox.InsertItems(self.surgeme,0)
        
    def DisplayAnnotation(self):
        self.m_listbox.Clear()
        self.m_listbox.InsertItems(self.surgeme,0)
        
if __name__ =='__main__':
    # The size of display area and the path of video file, the annotation file will be in the same dictionary will the video file
    VideoDisplaySize = [960, 540]
    video_path = r'C:\Users\jyoth\OneDrive\Desktop\Forward\Data Collection\Taurus\data\S3\S3_T1_color.avi'
    fpath = video_path.split('.')
    write_path = fpath[0] + '_annot.txt'
    a = np.loadtxt(write_path,dtype = str,usecols = (0,1,2,3))
    b=a[:,0].tolist()
    c=a[:,1].tolist()
    d=a[:,2].tolist()
    e=a[:,3].tolist()
    displaylist=[]
    for i,j,l,k in zip(b,c,d,e):
        displaylist.append(str(i)+' '+str(j)+' '+str(l)+' '+str(k))    
    app = wx.App()
    frame = AnnotationTool2(None, video_path, write_path, VideoDisplaySize)
    frame.Show()
    app.MainLoop()
    del app