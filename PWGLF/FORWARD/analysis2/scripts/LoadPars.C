/** 
 * Set-up for a PROOF analysis job.   Make TProof object and load pars. 
 * 
 * @param nWorkers Number of workers 
 *
 * @deprecated Use TrainSetup instead 
 *
 * @return True on success 
 *
 * @ingroup pwglf_forward_scripts
 */
Bool_t
LoadPars(Int_t nWorkers=4)
{
  const char* option = nWorkers <= 0 ? "" : Form("workers=%d", nWorkers);
  TProof::Open(option);
  const char* pkgs[] = { "STEERBase", "ESD", "AOD", "ANALYSIS", 
			 "ANALYSISalice", "PWGLFforward2", 0};
  const char** pkg = pkgs;
  Bool_t ret = true;
  while (*pkg) { 
    if (gProof->UploadPackage(Form("${ALICE_PHYSICS}/%s.par",*pkg)) < 0) {
      Error("LoadPars", "Failed to upload package %s", *pkg);
      ret = false;
      continue;
    }
    if (gProof->EnablePackage(*pkg) < 0) { 
      Error("LoadPars", "Failed to enable package %s", *pkg);
      ret = false;
      continue;
    }
    pkg++;
  }
  return ret;
}
//
// EOF
//
