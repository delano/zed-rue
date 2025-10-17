use zed_extension_api as zed;

struct RueExtension;

impl zed::Extension for RueExtension {
    fn new() -> Self {
        Self
    }

    fn language_server_command(
        &mut self,
        _language_server_id: &zed::LanguageServerId,
        _worktree: &zed::Worktree,
    ) -> zed::Result<zed::Command> {
        // TODO: Implement language server integration if needed
        // For now, we'll rely on existing Ruby LSP for the logic section
        Err("No language server configured yet".to_string())
    }
}

zed::register_extension!(RueExtension);
