#ifndef AliAnalysisTaskEmcalHfeTagging_H
#define AliAnalysisTaskEmcalHfeTagging_H

class TH1;
class TH2;
class TH3;
class TH3F;
class TTree;
class THnSparse;
class TClonesArray;
class TArrayI;
class AliAODEvent;
class AliJetContainer;
class AliParticleContainer;
class AliPIDResponse;
class AliHFEcontainer;
class AliHFEcuts;
class AliHFEpid;
class AliHFEpidQAmanager;
class AliCFManager;
class AliEventCuts;
class AliMultiEventInputHandler;
class AliAODMCHeader;

#include "AliAnalysisTaskEmcalJet.h"



class AliAnalysisTaskEmcalHfeTagging : public AliAnalysisTaskEmcalJet {
public:
    
    enum JetShapeType {
        kMCTrue = 0,   // generated jets only
        kTrueDet =1,  // detector and generated jets
        kData   = 2,  // raw data
        kDetEmb = 3,  //detector embedded jets
        kDetEmbPart = 4,
        kPythiaDef = 5,
        kDetEmbPartPythia=6,
        kGenOnTheFly = 7
    };
    enum JetShapeSub {
        kNoSub = 0,
        kConstSub = 1,
        kDerivSub = 2
    };
    enum JetSelectionType {
        kInclusive = 0,
        kRecoil = 1
    };
    
    enum DerivSubtrOrder {
        kSecondOrder = 0,
        kFirstOrder = 1
    };
    
    enum JetShapeTree {
        kAll = 0,
        kInclElec = 1,
        kPhotElec = 2,
        kNoElec = 3
    };
    
    AliAnalysisTaskEmcalHfeTagging();
    AliAnalysisTaskEmcalHfeTagging(const char *name);
    virtual ~AliAnalysisTaskEmcalHfeTagging();
    
    void                                UserCreateOutputObjects();
    void                                Terminate(Option_t *option);
    
    //Setters
    void SetJetContainer(Int_t c)                             { fContainer     = c   ; }
    void SetMinFractionShared(Double_t f)                     { fMinFractionShared = f   ; }
    void SetJetShapeType(JetShapeType t)                      { fJetShapeType       = t   ; }
    void SetJetShapeSub(JetShapeSub t)                        { fJetShapeSub     = t   ; }
    void SetJetSelection(JetSelectionType t)                  { fJetSelection    = t   ; }
    void SetJetPtThreshold(Float_t f)                         { fPtThreshold     = f   ; }
    void SetRMatching(Float_t f)                              { fRMatching = f ;}
    void SetSelectShapes(Int_t c)                                {fSelectedShapes = c;}
    void SetPtTriggerSelections(Float_t minpT, Float_t maxpT) { fminpTTrig = minpT; fmaxpTTrig = maxpT; }
    void SetAngularWindowRecoilJet (Float_t t)                {fangWindowRecoil = t; }
    Float_t GetMinPtTriggerSelection()                        {return fminpTTrig;}
    Float_t GetMaxPtTriggerSelection()                        {return fmaxpTTrig;}
    void SetCentralitySelectionOn(Bool_t t)                   { fCentSelectOn = t;}
    void SetOneConstSelectionOn(Bool_t t)                     { fOneConstSelectOn =t;}
    void SetMinCentrality(Float_t t)                          { fCentMin = t ; }
    void SetMaxCentrality(Float_t t)                          { fCentMax = t ; }
    void SetSemigoodCorrect(Int_t yesno)                 {fSemigoodCorrect=yesno;}
    void SetHolePos(Float_t poshole)                        { fHolePos = poshole; }
    void SetHoleWidth(Float_t holewidth)                  { fHoleWidth = holewidth; }
    void SetDerivativeSubtractionOrder(Int_t c)              {fDerivSubtrOrder = c;}
    void SetJetShapeTree(Int_t c)                            {fJetShapeTree = c;}
    
protected:
    Bool_t                              RetrieveEventObjects();
    Bool_t                              Run();
    Bool_t                              FillHistograms();
    
    void                                GetNumberOfElectrons(AliEmcalJet *jet,Int_t jetContNb, Int_t nMother, Double_t listMother[],  Int_t &nIncElec,  Int_t &nPhotElec);
    void                                GetNumberOfTrueElectrons(AliEmcalJet *jet,Int_t jetContNb, Int_t nMother, Double_t listMother[], Int_t &nTrueElec,  Int_t &nTrueHFElec);
    Int_t                               GetNumberOfPairs(AliAODTrack *track,const AliVVertex *pVtx, Int_t nMother, Double_t listMother[]);
    Bool_t                              IsFromHFdecay(AliAODMCParticle *particle);
    Bool_t                              IsFromLMdecay(AliAODMCParticle *particle);
    Bool_t                              IsPrimary(AliAODMCParticle *particle);
    Bool_t                              InclElecTrackCuts(const AliVVertex *pVtx,AliAODTrack *ietrack, Int_t nMother, Double_t listMother[]);
    Bool_t                              PhotElecTrackCuts(const AliVVertex *pVtx,AliAODTrack *aetrack, Int_t nMother, Double_t listMother[]);
    Float_t                             GetJetMass(AliEmcalJet *jet,Int_t jetContNb);
    Float_t                             Angularity(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetAngularity(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             Coronna(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetCoronna(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             PTD(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetpTD(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             Circularity(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetCircularity(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             LeSub(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetLeSub(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             GetJetNumberOfConstituents(AliEmcalJet *jet,Int_t jetContNb);
    Float_t                             GetSigma2(AliEmcalJet *jet, Int_t jetContNb);
    Float_t                             Sigma2(AliEmcalJet *jet, Int_t jetContNb);
    
    Int_t                               SelectTrigger(Float_t minpT, Float_t maxpT);
    Double_t                            RelativePhi(Double_t mphi, Double_t vphi);
    
    AliAODEvent                         *fAOD;                  //! AOD object
    AliVEvent                           *fVevent;               //! VEvent
    AliPIDResponse                      *fpidResponse;          //! PID response
    TClonesArray                        *fTracksTender;        //Tender for tracks
    const AliVVertex                    *pVtx;                   // z vertex
    const AliVVertex                    *spdVtx;                  //spd z vertez
    
    AliMCEvent                          *fMC;                   //! MC object
    AliStack                            *fStack;                              //! stack
    AliAODMCParticle                    *fMCparticle;            //! MC particle
    TClonesArray                        *fMCarray;               //! MC array
    AliAODMCHeader                      *fMCheader;
    
    
    Int_t                               fContainer;              // jets to be analyzed 0 for Base, 1 for subtracted.
    Float_t                             fMinFractionShared;          // only fill histos for jets if shared fraction larger than X
    JetShapeType                        fJetShapeType;               // jet type to be used
    JetShapeSub                         fJetShapeSub;                // jet subtraction to be used
    JetSelectionType                    fJetSelection;               // Jet selection: inclusive/recoil jet
    Float_t                             fShapesVar[19];                  // jet shapes used for the tagging
    Float_t                             fPtThreshold;
    Float_t                             fRMatching;
    Int_t                               fSelectedShapes;                //chose set of shapes
    Float_t                             fminpTTrig;                   //min - max pT for trigger particle in case of recoil jet
    Float_t                             fmaxpTTrig;
    Float_t                             fangWindowRecoil;             //angular window for btb recoil analysis
    Int_t                               fSemigoodCorrect;             //if==1 we run over semigood runs
    Float_t                             fHolePos;                          //position in radians of the bad TPC sector
    Float_t                             fHoleWidth;                       //width of the hole in radians
    Bool_t                              fCentSelectOn;                // switch on/off centrality selection
    Float_t                             fCentMin;                     // min centrality value
    Float_t                             fCentMax;                     // max centrality value
    Bool_t                              fOneConstSelectOn;                // switch on/off one constituent selection
    Int_t                               fDerivSubtrOrder;
    Int_t                               fJetShapeTree;                  // selection of the tree
    
    Double_t                            fAssPtCut;                     // pt cut for associated electron
    Int_t                               fITSncut;                       // ITC number of clusters for tagged electrons
    Int_t                               fAssTPCnCut;                    // TPC number of clusters for associated electron
    Int_t                               fTPCnCut;                       // TPC number of clusters for tagged electron
    Bool_t                              fAssITSrefitCut;                // ITS refit for associated electron
    Bool_t                              fUseTender;                     // Use tender
    Double_t                            fSigmaTOFcut;                   // sigma TOF cut |sigma_TOF|< cut
    Double_t                            fSigmaTPCcut;                   // sigma TPC cut - cut < sigma_TPC < 3
    Double_t                            fDcaXYcut;                      //DCA_xy cut
    Double_t                            fDcaZcut;                      //DCA_xy cut
    Double_t                            fIMcut;                       //invariant mass cut
    
    TH2F                                *fh2ResponseUW;
    TH2F                                *fh2ResponseW;
    TH2F                                *fPhiJetCorr6;
    TH2F                                *fPhiJetCorr7;
    TH2F                                *fEtaJetCorr6;
    TH2F                                *fEtaJetCorr7;
    TH2F                                *fPtJetCorr;
    TH1F                                *fPtJet;
    TH2F                                *fhpTjetpT; //control p[lot fo the recoil analysis
    TH1F                                *fhPt;
    TH1F                                *fhPhi;
    TH2F                                *fNbOfConstvspT;
    TH2F                                *fnTPCnTOFnocut;
    TH2F                                *fnTPCnocut;
    TH2F                                *fnTOFnocut;
    TH2F                                *fnTPCcut;
    TH2F                                *fnULSmLSpairsPerElectron;
    TH2F                                *fInvmassLS;
    TH2F                                *fInvmassULS;
    TH1I                                *fnPartPerJet;
    TH1F                                *fnElecOverPartPerJet;
    TH1I                                *fnInclElecPerJet;
    TH1I                                *fnPhotElecPerJet;
    TH1I                                *fnIncSubPhotElecPerJet;
    TH1I                                *fnTrueElecPerJet;
    TH1I                                *fnTrueHFElecPerJet;
    TH2F                                *fPi0Pt;
    TH2F                                *fEtaPt;
    
    TTree                               *fTreeObservableTagging;  // Tree with tagging variables subtracted MC or true MC or raw
    
private:
    AliAnalysisTaskEmcalHfeTagging(const AliAnalysisTaskEmcalHfeTagging&);            // not implemented
    AliAnalysisTaskEmcalHfeTagging &operator=(const AliAnalysisTaskEmcalHfeTagging&); // not implemented
    
    ClassDef(AliAnalysisTaskEmcalHfeTagging, 6)
};
#endif

