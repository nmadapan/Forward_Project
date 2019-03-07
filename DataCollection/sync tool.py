# -*- coding: utf-8 -*-
"""
Created on Tue Jan  12

@author: Jyothsna P B
"""

import wx
import wx.xrc
import cv2

class AnnotationTool2 ( wx.Frame ):
    
    def __init__( self, parent, video_path1, video_path2, video_path3, write_path, VideoDisplaySize ):
        # Define surgeme list and initial parameters
        self.WindowWidth = VideoDisplaySize[0] + 440
        self.WindowHeight = VideoDisplaySize[1] + 190
        
        self.ImWidth = int(VideoDisplaySize[0]/3)
        self.ImHeight = VideoDisplaySize[1]
        self.FrameNumber = 0
        self.FrameNumber1 = 0
        self.FrameNumber2 = 0
        self.FrameTime = 33
        self.FrameTime1 = 33
        self.FrameTime2 = 33
        self.video_path1 = video_path1
        self.video_path2 = video_path2
        self.video_path3 = video_path3
        self.write_path = write_path
        self.OneRow = []    
        self.OneAnnotation = ''
        self.AnnotationList = []
        
        wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = wx.EmptyString, pos = wx.DefaultPosition, \
                     size = wx.Size( self.WindowWidth, self.WindowHeight ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
        self.SetSizeHints( wx.DefaultSize, wx.DefaultSize )   
                
        #Define the framework
        bSizer1 = wx.BoxSizer( wx.VERTICAL )        
        bSizer2 = wx.BoxSizer( wx.HORIZONTAL )
        bSizer3 = wx.StaticBoxSizer( wx.StaticBox( self, wx.ID_ANY, u"Play Videos" ), wx.VERTICAL )  
        bSizer4 = wx.BoxSizer( wx.HORIZONTAL )
        bSizer5 = wx.BoxSizer( wx.HORIZONTAL )
        bSizer6 = wx.BoxSizer( wx.HORIZONTAL )
        
        bSizer2.SetMinSize( wx.Size( -1,20 ) ) 
        self.m_load = wx.Button( self, wx.ID_ANY, u"Load", wx.DefaultPosition, wx.DefaultSize, 0 )
        bSizer2.Add( self.m_load, 1, wx.ALL|wx.EXPAND, 5 )
        
        self.Inform_bar = wx.TextCtrl( self, wx.ID_ANY, u"Information is shown here", \
                                wx.DefaultPosition, wx.DefaultSize, wx.TE_CENTRE|wx.TE_READONLY )
        self.Inform_bar.SetBackgroundColour( wx.SystemSettings.GetColour( wx.SYS_COLOUR_MENU ) )        
        bSizer2.Add( self.Inform_bar, 3, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, 5 )                
        bSizer1.Add( bSizer2, 0, wx.EXPAND, 5 )          
              
        
        self.m_bitmap1 = wx.StaticBitmap( bSizer3.GetStaticBox(), wx.ID_ANY, wx.NullBitmap, \
                                  wx.DefaultPosition, wx.Size(self.ImWidth,self.ImHeight), 0 )
        bSizer4.Add( self.m_bitmap1, 0, wx.ALL, 5 )
        
        self.m_bitmap2 = wx.StaticBitmap( bSizer3.GetStaticBox(), wx.ID_ANY, wx.NullBitmap, \
                                  wx.DefaultPosition, wx.Size(self.ImWidth,self.ImHeight ), 0 )
        bSizer4.Add( self.m_bitmap2, 0, wx.ALL, 5 )
        
        self.m_bitmap3 = wx.StaticBitmap( bSizer3.GetStaticBox(), wx.ID_ANY, wx.NullBitmap, \
                                  wx.DefaultPosition, wx.Size(self.ImWidth,self.ImHeight ), 0 )
        bSizer4.Add( self.m_bitmap3, 0, wx.ALL, 5 )
        
        bSizer5.SetMinSize( wx.Size( -1,20 ) ) 
        self.m_slider = wx.Slider( self, wx.ID_ANY, 0, 0, 4000, wx.DefaultPosition, wx.Size( -1,40 ), wx.SL_LABELS|wx.SL_TOP )
        self.m_slider.SetMinSize( wx.Size( -1,30 ) )
        self.m_slider.SetMaxSize( wx.Size( -1,50 ) )
        bSizer5.Add( self.m_slider, 1, wx.ALL|wx.EXPAND, 5 )     
        self.m_slider1 = wx.Slider( self, wx.ID_ANY, 0, 0, 4000, wx.DefaultPosition, wx.Size( -1,40 ), wx.SL_LABELS|wx.SL_TOP )
        self.m_slider1.SetMinSize( wx.Size( -1,30 ) )
        self.m_slider1.SetMaxSize( wx.Size( -1,50 ) )
        bSizer5.Add( self.m_slider1, 1, wx.ALL|wx.EXPAND, 5 )
        self.m_slider2 = wx.Slider( self, wx.ID_ANY, 0, 0, 4000, wx.DefaultPosition, wx.Size( -1,40 ), wx.SL_LABELS|wx.SL_TOP )
        self.m_slider2.SetMinSize( wx.Size( -1,30 ) )
        self.m_slider2.SetMaxSize( wx.Size( -1,50 ) )
        bSizer5.Add( self.m_slider2, 1, wx.ALL|wx.EXPAND, 5 )
        self.m_buttonBefore = wx.Button( self, wx.ID_ANY, u"Last Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonBefore, 1, wx.ALL, 5 )
        self.m_toggleBtn = wx.ToggleButton( self, wx.ID_ANY, u"Pause Video 1", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_toggleBtn, 1, wx.ALL, 5)
        self.m_buttonNext = wx.Button( self, wx.ID_ANY, u"Next Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonNext, 1, wx.ALL, 5 )
        self.m_buttonBefore1 = wx.Button( self, wx.ID_ANY, u"Last Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonBefore1, 1, wx.ALL, 5 )           
        self.m_toggleBtn1 = wx.ToggleButton( self, wx.ID_ANY, u"Pause Video 2", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_toggleBtn1, 1, wx.ALL, 5) 
        self.m_buttonNext1 = wx.Button( self, wx.ID_ANY, u"Next Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonNext1, 1, wx.ALL, 5 )
        self.m_buttonBefore2 = wx.Button( self, wx.ID_ANY, u"Last Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonBefore2, 1, wx.ALL, 5 )
        self.m_toggleBtn2 = wx.ToggleButton( self, wx.ID_ANY, u"Pause Video 3", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_toggleBtn2, 1, wx.ALL, 5)       
        self.m_buttonNext2 = wx.Button( self, wx.ID_ANY, u"Next Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add( self.m_buttonNext2, 1, wx.ALL, 5 )
        self.m_cf = wx.Button( self, wx.ID_ANY, u"Choose Right Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_cf, 1, wx.ALL, 5)
        self.m_cf1 = wx.Button( self, wx.ID_ANY, u"Choose Left Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_cf1, 1, wx.ALL, 5)
        self.m_cf2 = wx.Button( self, wx.ID_ANY, u"Choose OSCR Frame", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_cf2, 1, wx.ALL, 5)
        self.m_delete = wx.Button( self, wx.ID_ANY, u"Delete Last Record", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_delete, 1, wx.ALL, 5)
        self.m_Write = wx.Button( self, wx.ID_ANY, u"Write to File", wx.DefaultPosition, wx.Size( -1,-1 ), 0 )
        bSizer6.Add(self.m_Write, 1, wx.ALL, 5)
        sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( bSizer3.GetStaticBox(), wx.ID_ANY, \
                                             u"Sync Record" ), wx.HORIZONTAL )
        sbSizer2.SetMinSize( wx.Size( -1,-1 ) )
        self.AnnotationArea = wx.TextCtrl( sbSizer2.GetStaticBox(), wx.ID_ANY, wx.EmptyString, \
                                    wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE|wx.TE_READONLY )
        self.AnnotationArea.SetFont( wx.Font( wx.NORMAL_FONT.GetPointSize(), 70, 90, 90, False, "Calibri" ) )        
        sbSizer2.Add( self.AnnotationArea, 1, wx.ALL|wx.EXPAND|wx.ALIGN_CENTER_HORIZONTAL, 5 )
        bSizer4.Add( sbSizer2, 1, wx.EXPAND, 5 )
        
        bSizer3.Add( bSizer4, 1, wx.EXPAND, 5 )        
        bSizer1.Add( bSizer3, 15, wx.EXPAND, 5 )
        bSizer1.Add( bSizer5, 1, wx.EXPAND, 5 )
        bSizer1.Add( bSizer6, 1, wx.EXPAND, 5 )    
        
        self.SetSizer( bSizer1 )
        self.Layout()
        timer_id = wx.NewId() 
        self.m_timer = wx.Timer()
        self.m_timer.SetOwner( self, timer_id )
        timer_id1 = wx.NewId() 
        self.m_timer1 = wx.Timer()
        self.m_timer1.SetOwner( self, timer_id1 )
        timer_id2 = wx.NewId() 
        self.m_timer2 = wx.Timer()
        self.m_timer2.SetOwner( self, timer_id2 )
        
        self.Centre( wx.BOTH )
        
        # Connect Events
        self.m_load.Bind( wx.EVT_BUTTON, self.OnLoad )
        self.m_Write.Bind( wx.EVT_BUTTON, self.SurgemeWrite )
        self.m_delete.Bind( wx.EVT_BUTTON, self.RecordDelete )
        
        self.m_cf.Bind( wx.EVT_BUTTON, self.Choose_Frame )
        self.m_cf1.Bind( wx.EVT_BUTTON, self.Choose_Frame1 )
        self.m_cf2.Bind( wx.EVT_BUTTON, self.Choose_Frame2 )
        self.m_slider.Bind( wx.EVT_SCROLL, self.OnSliderScroll )
        self.m_slider1.Bind( wx.EVT_SCROLL, self.OnSliderScroll1 )
        self.m_slider2.Bind( wx.EVT_SCROLL, self.OnSliderScroll2 )
        self.m_toggleBtn.Bind( wx.EVT_TOGGLEBUTTON, self.Play_Pause )
        self.m_toggleBtn1.Bind( wx.EVT_TOGGLEBUTTON, self.Play_Pause1 )
        self.m_toggleBtn2.Bind( wx.EVT_TOGGLEBUTTON, self.Play_Pause2 )
        
        self.Bind( wx.EVT_TIMER, self.OnTime, id = timer_id )
        self.Bind( wx.EVT_TIMER, self.OnTime1, id = timer_id1 )
        self.Bind( wx.EVT_TIMER, self.OnTime2, id = timer_id2 )
        
        self.m_buttonNext.Bind( wx.EVT_BUTTON, self.NextFrame )
        self.m_buttonBefore.Bind( wx.EVT_BUTTON, self.LastFrame )
        self.m_buttonNext1.Bind( wx.EVT_BUTTON, self.NextFrame1 )
        self.m_buttonBefore1.Bind( wx.EVT_BUTTON, self.LastFrame1 )
        self.m_buttonNext2.Bind( wx.EVT_BUTTON, self.NextFrame2 )
        self.m_buttonBefore2.Bind( wx.EVT_BUTTON, self.LastFrame2 )            
        
        #Set flags
        self.PAUSE_FLAG = False
        self.PAUSE_FLAG1 = False
        self.PAUSE_FLAG2 = False
        self.PROCESSING_FLAG = False
        
    def __del__( self ):
        self.m_timer.Stop()
        self.m_timer1.Stop()
        self.m_timer2.Stop()
        pass    
        
    # Virtual event handlers, overide them in your derived class
    def OnLoad( self, event ):
        self.videoCapture = cv2.VideoCapture(self.video_path1)
        self.videoCapture1 = cv2.VideoCapture(self.video_path2)
        self.videoCapture2 = cv2.VideoCapture(self.video_path3)
        if(self.videoCapture == None):
            wx.SafeShowMessage('start', 'Open Video 1 Failed')
            return
        if(self.videoCapture1 == None):
            wx.SafeShowMessage('start', 'Open Video 2 Failed')
            return
        if(self.videoCapture2 == None):
            wx.SafeShowMessage('start', 'Open Video 3 Failed')
            return
        self.TotalFrame = self.videoCapture.get(cv2.CAP_PROP_FRAME_COUNT)
        self.TotalFrame1 = self.videoCapture1.get(cv2.CAP_PROP_FRAME_COUNT)
        self.TotalFrame2 = self.videoCapture2.get(cv2.CAP_PROP_FRAME_COUNT)
        self.m_slider.SetMax(int(self.TotalFrame))
        self.m_slider1.SetMax(int(self.TotalFrame1))
        self.m_slider2.SetMax(int(self.TotalFrame2))
        self.fps = self.videoCapture.get(cv2.CAP_PROP_FPS)
        self.fps1 = self.videoCapture1.get(cv2.CAP_PROP_FPS)
        self.fps2 = self.videoCapture2.get(cv2.CAP_PROP_FPS)
        self.FrameTime = 1000 / self.fps
        self.FrameTime1 = 1000 / self.fps1
        self.FrameTime2 = 1000 / self.fps2
        self.m_timer.Start(self.FrameTime)
        self.m_timer1.Start(self.FrameTime1)
        self.m_timer2.Start(self.FrameTime2)
        self.Inform_bar.SetValue('Videos Loaded')
        self.PROCESSING_FLAG = True
        event.Skip()
        
    def Choose_Frame( self, event ):
        if self.PROCESSING_FLAG:
            if not self.OneRow:
                FN = str(int(self.FrameNumber))
                self.OneRow.append(FN)
                self.Inform_bar.SetValue('Sync for Right Video starts at Frame : ' + FN + '. Please choose Left Video frame')
            else:
                self.Inform_bar.SetValue('Please choose only Right video frame now!')
        else:
            self.Inform_bar.SetValue('Please load Video 1!')
        event.Skip()
        
    def Choose_Frame1( self, event ):
        if self.PROCESSING_FLAG:
            if len(self.OneRow) == 1:
                FN = str(int(self.FrameNumber1))
                self.OneRow.append(FN)
                self.Inform_bar.SetValue('Sync for Left Video starts at Frame : ' + FN + '. Please choose OSCR Video frame')
            else:
                self.Inform_bar.SetValue('Please choose only Left video frame now!')
        else:
            self.Inform_bar.SetValue('Please load Video 1!')
        event.Skip()
        
    def Choose_Frame2( self, event ):
        if self.PROCESSING_FLAG:
            if len(self.OneRow) == 2:
                FN = str(int(self.FrameNumber2))
                self.OneRow.append(FN)
                self.Inform_bar.SetValue('Sync for OSCR Video starts at Frame : ' + FN + '. Please Write to File')
            else:
                self.Inform_bar.SetValue('Please choose only OSCR video frame now!')
        else:
            self.Inform_bar.SetValue('Please load Video 1!')
        event.Skip()
    
    # Write an annotation to file and display it on annotation window
    def SurgemeWrite( self, event ):
        if self.PROCESSING_FLAG:
            # if the complete annotation is not created
            if len(self.OneRow) == 3:
                self.OneRow.append(str(fpath[-1]))
                for item in self.OneRow:
                    self.OneAnnotation += (item + ' ')
                self.AnnotationList.append(self.OneAnnotation + '\n')
                self.MyFileWriting()
                self.AnnotationArea.AppendText(self.AnnotationList[-1])
                # initilize the recording buffer
                self.OneRow = []
                self.OneAnnotation = ''
            # else if the complete annotation was created by 'create annotation' function
            elif len(self.OneRow) == 4:
                #self.OneAnnotation = self.m_Annotext.GetLineText(0)
                self.AnnotationList.append(self.OneAnnotation + '\n')
                self.AnnotationArea.AppendText(self.AnnotationList[-1])
                self.MyFileWriting()  
                self.OneRow = []
                self.OneAnnotation = ''                  
            else:
                self.Inform_bar.SetValue('please choose to create record')
        else:
            self.Inform_bar.SetValue('Please load the video!')
        event.Skip()
    
    # delete the last annotation record
    def RecordDelete( self, event ):
        if self.AnnotationList:
            self.AnnotationList.pop()
            self.MyFileWriting()
            self.DisplayAnnotation()
        else:
            self.Inform_bar.SetValue('No record can be deleted!')
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
            
    def OnSliderScroll1( self, event ):
        if self.PROCESSING_FLAG:
            self.FrameNumber1 = self.m_slider1.GetValue()
            self.videoCapture1.set(cv2.CAP_PROP_POS_FRAMES, self.FrameNumber1)
            success1, self.CurrentFrame1 = self.videoCapture1.read()
            if(success1):
                self.MyImshow1()
        else:
            event.Skip()
            
    def OnSliderScroll2( self, event ):
        if self.PROCESSING_FLAG:
            self.FrameNumber2 = self.m_slider2.GetValue()
            self.videoCapture2.set(cv2.CAP_PROP_POS_FRAMES, self.FrameNumber2)
            success2, self.CurrentFrame2 = self.videoCapture2.read()
            if(success2):
                self.MyImshow2()
        else:
            event.Skip()
        
    # toggle the playing and pausing condition of the video
    def Play_Pause( self, event ):
        if self.PROCESSING_FLAG:
            self.PAUSE_FLAG = event.GetEventObject().GetValue()
            if self.PAUSE_FLAG:
                self.m_timer.Stop()
                event.GetEventObject().SetLabel("Play Video 1")
            else:
                self.m_timer.Start(self.FrameTime)
                event.GetEventObject().SetLabel("Pause Video 1")
        else:
            self.Inform_bar.SetValue('Please load Video 1!')
        event.Skip()
        
    def Play_Pause1( self, event ):
        if self.PROCESSING_FLAG:
            self.PAUSE_FLAG1 = event.GetEventObject().GetValue()
            if self.PAUSE_FLAG1:
                self.m_timer1.Stop()
                event.GetEventObject().SetLabel("Play Video 2")
            else:
                self.m_timer1.Start(self.FrameTime1)
                event.GetEventObject().SetLabel("Pause Video 2")
        else:
            self.Inform_bar.SetValue('Please load Video 2!')
        event.Skip()
        
    def Play_Pause2( self, event ):
        if self.PROCESSING_FLAG:
            self.PAUSE_FLAG2 = event.GetEventObject().GetValue()
            if self.PAUSE_FLAG2:
                self.m_timer2.Stop()
                event.GetEventObject().SetLabel("Play Video 3")
            else:
                self.m_timer2.Start(self.FrameTime2)
                event.GetEventObject().SetLabel("Pause Video 3")
        else:
            self.Inform_bar.SetValue('Please load Video 3!')
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
            
    def OnTime1( self, event ):
        if self.PROCESSING_FLAG:
            success1, self.CurrentFrame1 = self.videoCapture1.read()
            self.FrameNumber1 = self.videoCapture1.get(cv2.CAP_PROP_POS_FRAMES)
            if(success1):
                self.MyImshow1()
        else:
            event.Skip()
            
    def OnTime2( self, event ):
        if self.PROCESSING_FLAG:
            success2, self.CurrentFrame2 = self.videoCapture2.read()
            self.FrameNumber2 = self.videoCapture2.get(cv2.CAP_PROP_POS_FRAMES)
            if(success2):
                self.MyImshow2()
        else:
            event.Skip()
    
    def LastFrame( self, event ):
        try:
            self.FrameNumber -= 1
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 1!')
        else:
            success, self.CurrentFrame = self.videoCapture.read()
            if(success) :
                self.MyImshow()
        event.Skip()   
        
        
    def NextFrame( self, event ):
        try:
            self.FrameNumber += 1
            self.videoCapture.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 1!')
        else:
            success, self.CurrentFrame = self.videoCapture.read()
            if(success) :
                self.MyImshow()
        event.Skip()
        
    def LastFrame1( self, event ):
        try:
            self.FrameNumber1 -= 1
            self.videoCapture1.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber1)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 2!')
        else:
            success1, self.CurrentFrame1 = self.videoCapture1.read()
            if(success1) :
                self.MyImshow1()
        event.Skip()   
        
        
    def NextFrame1( self, event ):
        try:
            self.FrameNumber1 += 1
            self.videoCapture1.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber1)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 2!')
        else:
            success1, self.CurrentFrame1 = self.videoCapture1.read()
            if(success1) :
                self.MyImshow1()
        event.Skip()
        
    def LastFrame2( self, event ):
        try:
            self.FrameNumber2 -= 1
            self.videoCapture2.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber2)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 3!')
        else:
            success2, self.CurrentFrame2 = self.videoCapture2.read()
            if(success2) :
                self.MyImshow2()
        event.Skip()   
        
        
    def NextFrame2( self, event ):
        try:
            self.FrameNumber2 += 1
            self.videoCapture2.set(cv2.CAP_PROP_POS_FRAMES , self.FrameNumber2)
        except AttributeError:
            self.Inform_bar.SetValue('Please load video 3!')
        else:
            success2, self.CurrentFrame2 = self.videoCapture2.read()
            if(success2) :
                self.MyImshow2()
        event.Skip()
            
    # Event handle defined, my functions below
    def MyImshow(self):
        self.CurrentFrame = cv2.resize(self.CurrentFrame, (self.ImWidth, self.ImHeight), interpolation = cv2.INTER_CUBIC)
        image = cv2.cvtColor(self.CurrentFrame, cv2.COLOR_BGR2RGB)
        pic = wx.Bitmap.FromBuffer(self.ImWidth, self.ImHeight, image)
        self.m_bitmap1.SetBitmap(pic)
        self.m_slider.SetValue(int(self.FrameNumber))
        
    def MyImshow1(self):
        self.CurrentFrame1 = cv2.resize(self.CurrentFrame1, (self.ImWidth, self.ImHeight), interpolation = cv2.INTER_CUBIC)
        image1 = cv2.cvtColor(self.CurrentFrame1, cv2.COLOR_BGR2RGB)
        pic1 = wx.Bitmap.FromBuffer(self.ImWidth, self.ImHeight, image1) 
        self.m_bitmap2.SetBitmap(pic1)
        self.m_slider1.SetValue(int(self.FrameNumber1))
        
    def MyImshow2(self):
        self.CurrentFrame2 = cv2.resize(self.CurrentFrame2, (self.ImWidth, self.ImHeight), interpolation = cv2.INTER_CUBIC)
        image2 = cv2.cvtColor(self.CurrentFrame2, cv2.COLOR_BGR2RGB)
        pic2 = wx.Bitmap.FromBuffer(self.ImWidth, self.ImHeight, image2) 
        self.m_bitmap3.SetBitmap(pic2)
        self.m_slider2.SetValue(int(self.FrameNumber2))
        
    def MyFileWriting(self):
        f = open(self.write_path, 'w')
        for annot in self.AnnotationList:
            f.write(annot)
        f.close()
        
    def DisplayAnnotation(self):
        self.AnnotationArea.Clear()
        for annot in self.AnnotationList:
            self.AnnotationArea.AppendText(annot)

        
if __name__ =='__main__':
    # The size of display area and the path of video file, the annotation file will be in the same dictionary will the video file
    VideoDisplaySize = [1260, 630]
    vid = r'C:\Users\jyoth\OneDrive\Desktop\Forward\Data Collection\Taurus_Sim\data\S1\S1_T1'
    fpath = vid.split('\\')
    video_path1 = vid + '_right_color.avi'
    video_path2 = vid + '_left_color.avi'
    video_path3 = vid + '_oscr.avi'
    write_path = r'C:\Users\jyoth\OneDrive\Desktop\Forward\Data Collection\Taurus_Sim\data\sync.txt'
    app = wx.App()
    frame = AnnotationTool2(None, video_path1, video_path2, video_path3, write_path, VideoDisplaySize)
    frame.Show()
    app.MainLoop()
    del app
